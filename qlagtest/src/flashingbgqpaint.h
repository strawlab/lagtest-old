#ifndef FLASHINGBGQPAINT_H
#define FLASHINGBGQPAINT_H

#include <QtGui>
#include <QWidget>
#include <QTimer>

#include "timemodel.h"
#include "ringbuffer.hpp"

class flashingBGQPaint : public QWidget
{
    Q_OBJECT
public:
    explicit flashingBGQPaint(int flipRate, TimeModel *clock, RingBuffer<screenFlip> *store);

public slots:
    void receiveStart();
    void receiveStop();

protected:
    void paintEvent(QPaintEvent *event);

    QRect r;
    bool drawWhiteBG;
    bool redraw;
    QTimer* timer;
    TimeModel *clock;
    RingBuffer<screenFlip> *store;

signals:
    
public slots:
    void flipColor();
    
};

#endif // FLASHINGBGQPAINT_H
