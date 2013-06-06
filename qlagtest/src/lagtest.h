#ifndef LAGTEST_H
#define LAGTEST_H

#include <QObject>


#include <vector>

class QSettings;
class QNetworkReply;

class LagTest : public QObject
{    
    Q_OBJECT
public:
    explicit LagTest(int clockSyncPeriod, int latencyUpdate, int screenFlipPeriod);
    virtual ~LagTest();

public slots:
    void receiveFlashAdruino();
    void recvVersionCheckFinished(QNetworkReply*reply);

public:
    static std::vector<QString> discoverComPorts();
    static int programArduino(QString avrDudePath, QString pathToFirmware, QString port=QString() );
    static QString makeUserSelectPort();

protected:
    bool loadSettings();
    bool testPort(QString port);
    void doNewVersionCheck();

    QSettings* settings;
};

#endif // LAGTEST_H
