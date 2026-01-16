#ifndef DATATHREAD_H
#define DATATHREAD_H

#include <QThread>
#include <QMutex>
#include <QDateTime>


class DataThread : public QThread
{
    Q_OBJECT
public:
    explicit DataThread(QObject *parent = nullptr);
    ~DataThread() override;


    void startWork(int interval = 2000);

    void stopWork();

signals:

    void sigSensorDataUpdate(float temp, float humi, QString time);

    void sigHeartbeatStateUpdate(QString state, QString time);

    void sigThreadError(QString errorInfo);

protected:
    void run() override;

private:
    bool m_isRunning;
    int m_collectInterval;
    QMutex m_mutex;
};

#endif // DATATHREAD_H
