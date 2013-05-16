#include "latencymodel.h"
#include <limits>

LatencyModel::LatencyModel(int ms_updateRate, TimeModel *tm, RingBuffer<double> *screenFlips, RingBuffer<clockPair> *clock_storage, RingBuffer<adcMeasurement> *adc_storage)
    : tm(tm), screenFlips(screenFlips), clock(clock_storage), adc(adc_storage), timer(0),
      latencyCnt(0), flipCnt(0)
{
    // Setup period update of the model
    connect(&(this->timer), SIGNAL(timeout()), this, SLOT( update() ));
    timer.start(ms_updateRate);

    //Initialize latency history
    for( int i=0; i < LatencyModel::latencyHistorySize; i++)
        this->latency[i] = -1.0;

    for( int i=0; i < LatencyModel::flipCnt; i++)
        this->flips[i] = numeric_limits<double>::max( );
}

void LatencyModel::update()
{
    qDebug("Updateing the latency model [%g]", this->tm->getCurrentTime() );

    //Read all new clock pairs and use them to update the time model
    clockPair cp;
    while(this->clock->canGet()){
        this->clock->get(&cp);
        this->tm->update(cp);
    }


    //Add new screen flips
    while(this->screenFlips->canGet()){
        this->screenFlips->get(&(this->flips[this->flipCnt]) );
        flipCnt = (flipCnt+1) % flipHistorySize;
    }

    adcMeasurement adc;
    while( this->findFlipp(&adc) ){
        this->latency[this->latencyCnt] = this->calculateLatency(adc);
        latencyCnt = (latencyCnt+1) % latencyHistorySize;
    }
}

double LatencyModel::calculateLatency(adcMeasurement adc)
{
    double detectedFlip = this->tm->toLocalTime( adc );
    //Find flip that is closest to this event, and calculate latency

    double closest = numeric_limits<double>::max();
    double latency;
    int screenSwitch = 0;

    for(int i=0; i < this->latencyHistorySize; i++){
        latency = sqrt( (detectedFlip - this->flips[i])*(detectedFlip - this->flips[i]) );
        if( latency < closest ){
            closest = latency;
            screenSwitch = i;
        }
    }

    qDebug("Flip@ %g, Closest Screen Switch@ %g, Latency [%g]", detectedFlip, flips[screenSwitch] ,closest );

    qDebug("Latency History [%g] [%g] [%g] [%g] [%g]", flips[0],flips[1],flips[2],flips[3],flips[4]);

    return closest;
}


bool LatencyModel::findFlipp(adcMeasurement* flip)
{
    uint8_t prev, curr, next;
    double diff;

    adcMeasurement t;
    //We need at least three elements in the adc ring buffer
    if( this->adc->get(&t) ){
        prev = t.adc;
    } else {
        return false;
    }
    if( this->adc->get(flip) ){
        curr = flip->adc;
    } else {
        return false;
    }
    if( this->adc->get(&t) ){
        next = t.adc;
    } else {
        return false;
    }

    bool found = false;
#define MAGIC_VALUE 10.0
    while( !found && this->adc->canGet() ){
        diff = sqrt( (double)(prev-next)*(prev-next) );

        if( diff > MAGIC_VALUE ) {
            qDebug("Found a flip [%d] [%d] [%d] @ %g", prev, curr, next, this->tm->toLocalTime( *flip ) );
            found = true;

            //Throw away some following ones to make sure it will not detect the same flip again
            this->adc->get(&t);
            this->adc->get(&t);
            this->adc->get(&t);

        } else {
            prev = curr;
            curr = next;
            *flip = t;
            this->adc->get(&t);
            next = t.adc;
            //qDebug("Next [%d], diff %g", next, diff);
        }
    }
    return found;
}
