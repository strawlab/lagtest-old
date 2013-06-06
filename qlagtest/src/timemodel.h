#ifndef TIMEMODEL_H
#define TIMEMODEL_H

#include <QElapsedTimer>
#include <stdint.h>

#include <Eigen/Dense>
using namespace std;
using namespace Eigen;

typedef struct{
    uint16_t adruino_ticks;
    uint32_t adruino_epoch;
    double local;
} clockPair;

enum flip_type { BLACK_TO_WHITE = 0, WHITE_TO_BLACK  = 1};
typedef struct{
    double local;
    enum flip_type type;
} screenFlip;

typedef struct{
    uint16_t adruino_ticks;
    uint32_t adruino_epoch;
    uint8_t adc;
} adcMeasurement;

class TimeModel
{
public:
    TimeModel();

    const static int clockHistory = 5;

    void testModelGenerator();
    double getCurrentTime();
    double toLocalTime( adcMeasurement adc);
    void update(clockPair cp);

private:
    QElapsedTimer*  timer;
    int cpCnt;

    MatrixXd A;
    VectorXd b;
    Vector2d x;
    double gain, offset;
};

#endif // TIMEMODEL_H
