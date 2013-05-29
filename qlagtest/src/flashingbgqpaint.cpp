#include "flashingbgqpaint.h"

flashingBGQPaint::flashingBGQPaint(int flipRate, TimeModel* clock, RingBuffer<screenFlip> *store) :
    QWidget(0),
    r(this->rect()),
    drawWhiteBG(true),
    clock(clock),
    store(store)
{
    this->timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(flipColor()));
    timer->setInterval(flipRate);
}

void flashingBGQPaint::flipColor()
{
    this->drawWhiteBG = !this->drawWhiteBG;
    emit update(); //Will produce a paint event, and make the screen update
}

void flashingBGQPaint::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(r, (this->drawWhiteBG) ? Qt::white : Qt::black );    

    screenFlip sf;
    sf.type = this->drawWhiteBG ? BLACK_TO_WHITE : WHITE_TO_BLACK;
    sf.local = this->clock->getCurrentTime();

    this->store->put( &sf );    
}

void flashingBGQPaint::receiveStart(){
    this->timer->start();
}

void flashingBGQPaint::receiveStop(){
    this->timer->stop();
    this->drawWhiteBG = false;
    emit update();
}
