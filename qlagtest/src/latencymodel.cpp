#include "latencymodel.h"
#include <limits>

LatencyModel::LatencyModel(int ms_updateRate, TimeModel *tm, RingBuffer<screenFlip> *screenFlips, RingBuffer<clockPair> *clock_storage, RingBuffer<adcMeasurement> *adc_storage)
    : QObject(0),
	tm(tm), screenFlips(screenFlips), clock(clock_storage), adc(adc_storage),
      timer(0),
      latencyCnt(0), flipCnt(0)
{
    // Setup period update of the model
	this->timer = new QTimer();
    connect( timer, SIGNAL(timeout()), this, SLOT( update() ));    
    this->timer->setInterval(ms_updateRate);
}

void LatencyModel::start()
{
    QTimer* t = new QTimer();
    t->setSingleShot(true);
    connect(t, SIGNAL(timeout()), this, SLOT( realStart()) );
    t->start(1000); //start one second later
}

void LatencyModel::realStart()
{
    qDebug("Starting latency model ...");
    this->resetHistory();
    this->timer->start();
}

void LatencyModel::resetHistory()
{
    qDebug("Reseting Latency Model History ...");
    //Initialize latency history
    for( int i=0; i < LatencyModel::latencyHistorySize; i++)
    {
        this->latency[i] = -1.0;
        for( int j=0; j < measurementWindowSize; j++ )
        {
            this->adcData[WHITE_TO_BLACK][i][j] = -1;
            this->adcData[BLACK_TO_WHITE][i][j] = -1;
        }
    }

    for( int i=0; i < latencyHistorySize; i++){
        this->latency[i] = -1.0;
    }

    this->measurementCnter[BLACK_TO_WHITE] = 0;
    this->measurementCnter[WHITE_TO_BLACK] = 0;
    this->nMeasurements[BLACK_TO_WHITE] = 0;
    this->nMeasurements[WHITE_TO_BLACK] = 0;

    this->lastLatency = -1.0;
}

void LatencyModel::reset()
{
    this->resetHistory();
}

void LatencyModel::update()
{
    double latency;

    qDebug("Updateing the latency model [%g]", this->tm->getCurrentTime() );

    //Read all new clock pairs and use them to update the time model
    clockPair cp;
    while(this->clock->canGet()){
        this->clock->get(&cp);
        this->tm->update(cp);
    }

    latency = -1.0;
    //Add new screen flips
    adcMeasurement adc;
    while( this->screenFlips->canGet() )
    {
        this->screenFlips->get(&(this->flips[this->flipCnt]) );

        if( this->findFlipp(&adc, this->flips[this->flipCnt] ) )
        {
            latency = this->calculateLatency( );
            qDebug("Latency [%g]", latency);

            this->latency[this->latencyCnt] = latency;
            latencyCnt = (latencyCnt+1) % latencyHistorySize;
            flipCnt = (flipCnt+1) % flipHistorySize;
        } else {
            //Leave this flip for the next time arround
            this->screenFlips->unget();
            break;
        }
    }      

    if( latency != -1.0 )
    {
        if( this->isStable(&this->lastLatency) ){
            emit signalStableLatency(this->lastLatency);
        } else {
            emit signalUnstableLatency();
        }
    }
}

double LatencyModel::calculateLatency()
{
    long avgADC[2][measurementWindowSize];

    //Calculate mean values
    int i,j,n, idx;
    double latency;

    if( this->detectdisplacedSensor() )
    {
        emit signalInvalidLatency();
        return -1;
    }

    if( this->nMeasurements[0] < this->nMeasurements[1]) {
        n = nMeasurements[0];
    } else {
        n = nMeasurements[1];
    }
    if( n > measurementHistoryLength )
        n = measurementHistoryLength;

    for(i = 0; i < measurementWindowSize; i++){
        avgADC[WHITE_TO_BLACK][i] = 0;
        avgADC[BLACK_TO_WHITE][i] = 0;
    }

    //char buffer[2000];

    for( i=0; i < n; i++)
    {
        for( j=0; j < measurementWindowSize; j++ )
        {
            avgADC[WHITE_TO_BLACK][j] += this->adcData[WHITE_TO_BLACK][i][j];
            avgADC[BLACK_TO_WHITE][j] += this->adcData[BLACK_TO_WHITE][i][j];   
        }

//        buffer[0] = 0;
//        sprintf(buffer,"W2B Sample [%d]\n", i);
//        for(int k = 0; k < measurementWindowSize; k++){
//            sprintf( &buffer[strlen(buffer)], " [%d] ", (this->adcData[WHITE_TO_BLACK][i][k]) );
//        }
//        qDebug(buffer);

//        buffer[0] = 0;
//        sprintf(buffer,"B2W Sample [%d]\n", i);
//        for(int k = 0; k < measurementWindowSize; k++){
//            sprintf( &buffer[strlen(buffer)], " [%d] ", (this->adcData[BLACK_TO_WHITE][i][k]) );
//        }
//        qDebug(buffer);
//        qDebug("#############");

    }

    idx = -1;
    for( j=0; j < measurementWindowSize; j++ )
    {
        if( avgADC[WHITE_TO_BLACK][j] < avgADC[BLACK_TO_WHITE][j]){
            idx = j;
            qDebug("Found a crossofer at idx %d", idx);
            break;
        }
    }

    if( (idx == 0) || (idx==measurementWindowSize) )
    {
        qWarning("Could not detect crossover!");
        latency = -1;
        emit signalInvalidLatency();
    } else {
        latency = this->sampleTimes[idx];
    }


    return latency;
}

bool LatencyModel::detectdisplacedSensor()
{
    int j;
    double windowAVG[2] = {0, 0};
    double windowSTD[2] = {0, 0};

    for( j=0; j < measurementWindowSize; j++ )
    {
        windowAVG[WHITE_TO_BLACK] += this->adcData[WHITE_TO_BLACK][this->measurementCnter[WHITE_TO_BLACK]][j];
        windowAVG[BLACK_TO_WHITE] += this->adcData[BLACK_TO_WHITE][this->measurementCnter[BLACK_TO_WHITE]][j];
    }
    windowAVG[WHITE_TO_BLACK] = windowAVG[WHITE_TO_BLACK] / (double) measurementWindowSize;
    windowAVG[BLACK_TO_WHITE] = windowAVG[BLACK_TO_WHITE] / (double) measurementWindowSize;

    for( j=0; j < measurementWindowSize; j++ )
    {
        windowSTD[WHITE_TO_BLACK] += sqrt( (windowAVG[WHITE_TO_BLACK]-this->adcData[WHITE_TO_BLACK][this->measurementCnter[WHITE_TO_BLACK]][j])*(windowAVG[WHITE_TO_BLACK]-this->adcData[WHITE_TO_BLACK][this->measurementCnter[WHITE_TO_BLACK]][j]) );
        windowSTD[BLACK_TO_WHITE] += sqrt( (windowAVG[BLACK_TO_WHITE]-this->adcData[BLACK_TO_WHITE][this->measurementCnter[BLACK_TO_WHITE]][j])*(windowAVG[BLACK_TO_WHITE]-this->adcData[BLACK_TO_WHITE][this->measurementCnter[BLACK_TO_WHITE]][j]) );
    }

    if( (windowSTD[WHITE_TO_BLACK] < windowAVG[WHITE_TO_BLACK]) || (windowSTD[BLACK_TO_WHITE] < windowAVG[BLACK_TO_WHITE]) )
    {
        qDebug("Too little difference in the measurement values ...");
        return true;
    } else {
        return false;
    }
}

bool LatencyModel::findFlipp(adcMeasurement* flip, screenFlip sf )
{   
    bool found = false;
    adcMeasurement s;

    qDebug("Trying to find a flip at %g", sf.local);
    //Consume all adc sample values till we get one taken after the flip
    while( !found && this->adc->canGet())
    {
        this->adc->get(&s);

        //last entry in the ringbuffer is the sample closest in time to the screen flip
        if( this->tm->toLocalTime(s) > sf.local ){
            found = true;
            //qDebug( "Found the closest samples after %d elements" );
        }
    }

    if( !found || (this->adc->unread() < measurementWindowSize) )
    {   //Could not find a sample close to the screen flip
        qWarning("Not enought adc sample data!");
        found = false;
    } else {
        //The WindowSize Previous elements in the adc ringbuffer are part of this sample window
        int i;
        adcMeasurement tsample;

        this->nMeasurements[sf.type] ++;
        this->measurementCnter[ sf.type ] = (this->measurementCnter[ sf.type ]+1)%measurementHistoryLength;

        this->adcData[sf.type][this->measurementCnter[sf.type]][0] = s.adc;
        this->sampleTimes[0] = this->tm->toLocalTime( s ) - sf.local;

        for(i=1; i < measurementWindowSize; i++){
            if( this->adc->get( &tsample  ) )
            {
                this->adcData[sf.type][this->measurementCnter[sf.type]][i] = tsample.adc;
                this->sampleTimes[i] = this->tm->toLocalTime( tsample ) - sf.local;
                //qDebug("E: %g, T: %g, D: %d", tsample.adruino_epoch, tsample.adruino_ticks, tsample.adc);
            } else {
                qCritical("Not enough data in the ADC ringbuffer");
            }

        }
    }

//    char buffer[10000];
//    buffer[0] = 0;
//    sprintf(buffer,"Timetable of adc Sample\n");
//    for(int k = 0; k < measurementWindowSize; k++){
//        sprintf( &buffer[strlen(buffer)], " [%g] ", (this->sampleTimes[k]) );
//    }
//    qDebug(buffer);

    return found;
}

bool LatencyModel::isStable(double* stableResult)
{
    double diff;
    double t;
    double mean;
    bool isStable = false;
    t = 0;
    int prev;

    //If the last 3 latency values dont differ in more than 10% the result is stable
    mean = 0;
    prev = this->latencyCnt;
    isStable = true;
    for(int i = 0; i < latencyStableSize; i++)
    {
        prev = (prev == 0) ? latencyHistorySize-1 : prev-1 ;
        t = this->latency[prev];

        if( mean != 0)
        {
            diff = sqrt( (t-mean)*(t-mean) );
            if( diff > 0.1*mean ){
                isStable = false;
                break;
            } else {
                mean = ( mean + t ) / 2.0;
            }
        } else {
            mean = t;
        }
    }

    if(isStable && stableResult){
        *stableResult = mean;
    }

    return isStable;
}
