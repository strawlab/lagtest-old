#ifndef LAGTESTSERIALPORTCOMM_H
#define LAGTESTSERIALPORTCOMM_H

#include <QObject>
#include <stdint.h>

// lagtest\firmware\lagtestino\lagtest_dataypes.h
typedef uint32_t epoch_dtype;
typedef struct {
    uint8_t cmd;    //either 'H', 'P', 'L', 'V'
    uint8_t value;
    epoch_dtype epoch;
    uint16_t ticks;
    uint8_t checksum;   //bytewise ( value + epoch + ticks ) % 256
} timed_sample_t;


class LagTestSerialPortComm : public QObject
{
    Q_OBJECT
public:
    explicit LagTestSerialPortComm(int bautRate);
    
signals:
    void finished();

public slots:
    void startCommunication();
    
private:
    int bautRate;
    bool readFrame(uint8_t *buffer, timed_sample_t* frame);
};

#endif // LAGTESTSERIALPORTCOMM_H
