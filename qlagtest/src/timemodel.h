#ifndef TIMEMODEL_H
#define TIMEMODEL_H

#include <QElapsedTimer>

class TimeModel
{
public:
    TimeModel();

    void testModelGenerator();
    double getCurrentTime();

private:
    QElapsedTimer*  timer;
};

#endif // TIMEMODEL_H
