#ifndef DATATHREAD_H
#define DATATHREAD_H

#include <QThread>
#include <QMutex>
#include <QDateTime>

// 后台线程：非UI线程，负责定时采集温湿度传感器数据、发送TCP心跳包、定时刷新设备状态
// 所有数据更新通过信号发送到主线程，主线程更新UI，遵守QT线程安全规则
class DataThread : public QThread
{
    Q_OBJECT
public:
    explicit DataThread(QObject *parent = nullptr);
    ~DataThread() override;

    // 启动线程：设置采集间隔（默认2秒）
    void startWork(int interval = 2000);
    // 停止线程
    void stopWork();

signals:
    // 传感器数据更新信号：发送到主线程
    void sigSensorDataUpdate(float temp, float humi, QString time);
    // 心跳包状态更新信号
    void sigHeartbeatStateUpdate(QString state, QString time);
    // 线程错误信号
    void sigThreadError(QString errorInfo);

protected:
    void run() override; // 线程入口函数

private:
    bool m_isRunning;    // 线程运行标志
    int m_collectInterval; // 采集间隔(ms)
    QMutex m_mutex;      // 互斥锁，保证线程安全
};

#endif // DATATHREAD_H
