#ifndef LAGTEST_H
#define LAGTEST_H

#include <QObject>


#include <vector>

class QSettings;
class QNetworkReply;

class LatencyModel;
class SerialPortHandler;
class Window;

class LagTest : public QObject
{    
    Q_OBJECT
public:
    explicit LagTest(int clockSyncPeriod, int latencyUpdate, int screenFlipPeriod);
    virtual ~LagTest();

public slots:
    void receiveFlashAdruino();
    void recvVersionCheckFinished(QNetworkReply*reply);
    void generateReport();
    void recvFlashAdruino();
    void recvShowLogWindow();

public:
    static std::vector<QString> discoverComPorts();
    static int programArduino(QString avrDudePath, QString pathToFirmware, QString port=QString() );
    static QString makeUserSelectPort();

protected:
    bool loadSettings();
    bool testPort(QString port);
    void doNewVersionCheck();
    void setupLogWindow();

    QSettings* settings;
    LatencyModel* lm;
    SerialPortHandler* serial;
    Window* w;
};

#endif // LAGTEST_H
