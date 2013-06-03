#ifndef LAGTEST_H
#define LAGTEST_H

#include <QObject>


#include <vector>

class QSettings;

class LagTest : public QObject
{    
    Q_OBJECT
public:
    explicit LagTest(int clockSyncPeriod, int latencyUpdate, int screenFlipPeriod);

public slots:
    void receiveFlashAdruino();

public:
    static std::vector<QString> discoverComPorts();
    static int programArduino(QString avrDudePath, QString pathToFirmware, QString port=QString() );
    static QString makeUserSelectPort();

protected:
    bool loadSettings();
    bool testPort(QString port);

    QSettings* settings;
};

#endif // LAGTEST_H
