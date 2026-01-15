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
        wait(3000); // 等待3秒，超时强制退出
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

        // ✅✅✅ Qt6终极完美解决方案 无任何歧义 100%编译通过
        // 原理：先生成 [0,1)的标准浮点随机数，再手动映射到指定区间，完美规避所有重载问题
        float tempOffset = (QRandomGenerator::global()->generateDouble() - 0.5) * 3.0; // -1.5 ~ +1.5
        float humiOffset = (QRandomGenerator::global()->generateDouble() - 0.5) * 6.0; // -3.0 ~ +3.0

        float temp = baseTemp + tempOffset;
        float humi = baseHumi + humiOffset;

        temp = qBound(18.0f, temp, 32.0f); // 温度范围18-32℃ 限制
        humi = qBound(30.0f, humi, 70.0f); // 湿度范围30-70% 限制
        QString currTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
        emit sigSensorDataUpdate(temp, humi, currTime);

        // 模拟发送TCP心跳包，状态更新
        emit sigHeartbeatStateUpdate(u8"心跳包发送成功", currTime);

        // 休眠采集间隔
        msleep(m_collectInterval);
    }

    qDebug() << "后台数据采集线程停止";
}
