#ifndef FLASHINGBACKGROUND_H
#define FLASHINGBACKGROUND_H

#include "openglwindow.h"
#include <ringbuffer.hpp>
#include <QTimer>

class TimeModel;

class FlashingBackground : public OpenGLWindow
{
    Q_OBJECT

public:
    FlashingBackground(int msec, TimeModel* clock, RingBuffer<double>* store);

    //void initialize();
    void render();

public slots:
    void flipColor();

private:
    bool drawWhiteBG;
    bool redraw;
    QTimer* timer;
    TimeModel* clock;
    RingBuffer<double>* store;
};

#endif // FLASHINGBACKGROUND_H
