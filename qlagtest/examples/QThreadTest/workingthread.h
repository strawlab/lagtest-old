#ifndef WORKINGTHREAD_H
#define WORKINGTHREAD_H

#include <QObject>

class workingThread : public QObject
{
    Q_OBJECT
public:
    workingThread(int* var);

private:
    int* var;

signals:
    void finished();
    
public slots:
    void doProcessing();
};

#endif // WORKINGTHREAD_H
