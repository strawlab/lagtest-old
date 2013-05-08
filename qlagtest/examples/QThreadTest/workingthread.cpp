#include "workingthread.h"
#include <QThread>

workingThread::workingThread(int* var) :
    QObject(0)
{
    this->var = var;
}

void workingThread::doProcessing()
{
    qDebug("In do Processing ... ");
    qDebug("worker: var = %d", *var);

    int var_tmp = *var;

    while(1)
    {
        if( *var != var_tmp){
            qDebug("worker: var changed to %d", *var);
            var_tmp = *var;
        }

        if( *var == 3)
        {
            qDebug("worker: this is enougth, setting var to 100");
            *var = 100;
            emit finished();
        }
    }
}
