#ifndef MAINAPP_H
#define MAINAPP_H

#include <QObject>

class mainApp : public QObject
{
    Q_OBJECT
public:
    explicit mainApp(QObject *parent = 0);
    void start();

signals:
    
public slots:
    void onThreadQuit();
    
};

#endif // MAINAPP_H
