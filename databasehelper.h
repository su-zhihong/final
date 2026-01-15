#ifndef DATABASEHELPER_H
#define DATABASEHELPER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QDateTime>
#include <QDebug>

// 单例模式的数据库工具类，封装所有数据库操作
class DataBaseHelper
{
public:
    // 获取单例对象
    static DataBaseHelper* getInstance();
    // 释放单例
    static void releaseInstance();

    // 初始化数据库（创建库+创建表）
    bool initDatabase();
    // 写入设备操作日志：设备名+操作类型+操作结果+备注
    void writeDeviceLog(const QString &devName, const QString &operType, const QString &result, const QString &note = "");
    // 写入设备状态：设备名+设备状态+更新时间
    void writeDeviceState(const QString &devName, const QString &state);
    // 写入传感器数据：温度+湿度+采集时间
    void writeSensorData(float temp, float humi);

private:
    // 单例模式：私有构造/析构/拷贝
    DataBaseHelper();
    ~DataBaseHelper();
    DataBaseHelper(const DataBaseHelper&) = delete;
    DataBaseHelper& operator=(const DataBaseHelper&) = delete;

    static DataBaseHelper* m_instance; // 单例对象
    QSqlDatabase m_db;                 // 数据库连接对象
};

#endif // DATABASEHELPER_H
