#include "flashingbackground.h"
#include "timemodel.h"
#include "QDebug"

FlashingBackground::FlashingBackground(int msec, TimeModel* clock, RingBuffer<double> *store)
    : drawWhiteBG(true), redraw(true),
        clock(clock),
        store(store)
{
    qDebug("Creating Flashing Background window ...");
    this->timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(flipColor()));
    timer->start(msec);
}

void FlashingBackground::flipColor()
{
    this->drawWhiteBG = !this->drawWhiteBG;
    this->redraw = true;
}

void FlashingBackground::render()
{
    if(this->redraw)
    {
        glViewport(0, 0, width(), height());

        if( this->drawWhiteBG ){
            glClearColor(1.0, 1.0, 1.0, 1.0);
            double d = this->clock->getCurrentTime();
            this->store->put( &d );
            qDebug("To White at %g", d );
        } else {
            glClearColor(0.0, 0.0, 0.0, 1.0);
            double d = this->clock->getCurrentTime();
            this->store->put( &d );
            qDebug("To Block at %g", d );
        }

        glClear(GL_COLOR_BUFFER_BIT);
        redraw = false;
    }
}
