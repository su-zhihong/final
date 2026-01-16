#include "datathread.h"
#include <QRandomGenerator>

DataThread::DataThread(QObject *parent)
    : QThread(parent)
    , m_isRunning(false)
    , m_collectInterval(2000)
{
}

DataThread::~DataThread()
{
    stopWork();
}

void DataThread::startWork(int interval)
{
    QMutexLocker locker(&m_mutex);
    m_collectInterval = interval;
    m_isRunning = true;
    if(!isRunning())
        start();
}

void DataThread::stopWork()
{
    QMutexLocker locker(&m_mutex);
    m_isRunning = false;
    if(isRunning())
    {
        quit();
        wait(3000);
    }
}

void DataThread::run()
{
    qDebug() << "后台数据采集线程启动，线程ID：" << QThread::currentThreadId();
    float baseTemp = 25.0f;
    float baseHumi = 50.0f;

    while (true)
    {
        {
            QMutexLocker locker(&m_mutex);
            if(!m_isRunning) break;
        }


        float tempOffset = (QRandomGenerator::global()->generateDouble() - 0.5) * 3.0; // -1.5 ~ +1.5
        float humiOffset = (QRandomGenerator::global()->generateDouble() - 0.5) * 6.0; // -3.0 ~ +3.0

        float temp = baseTemp + tempOffset;
        float humi = baseHumi + humiOffset;

        temp = qBound(18.0f, temp, 32.0f); // 温度范围18-32℃ 限制
        humi = qBound(30.0f, humi, 70.0f); // 湿度范围30-70% 限制
        QString currTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
        emit sigSensorDataUpdate(temp, humi, currTime);

        emit sigHeartbeatStateUpdate(u8"心跳包发送成功", currTime);


        msleep(m_collectInterval);
    }

    qDebug() << "后台数据采集线程停止";
}
