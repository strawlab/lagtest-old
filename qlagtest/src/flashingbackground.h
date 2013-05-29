#ifndef FLASHINGBACKGROUND_H
#define FLASHINGBACKGROUND_H

#include "timemodel.h"
#include "openglwindow.h"
#include <ringbuffer.hpp>
#include <QTimer>

class TimeModel;

class FlashingBackground : public OpenGLWindow
{
    Q_OBJECT

public:
    FlashingBackground(int msec, TimeModel* clock, RingBuffer<screenFlip> *store);

    //void initialize();
    void render();

public slots:
    void flipColor();

private:
    bool drawWhiteBG;
    bool redraw;
    QTimer* timer;
    TimeModel* clock;
    RingBuffer<screenFlip>* store;
};

#endif // FLASHINGBACKGROUND_H
