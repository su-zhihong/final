#ifndef DATABASEHELPER_H
#define DATABASEHELPER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QDateTime>
#include <QDebug>


class DataBaseHelper
{
public:

    static DataBaseHelper* getInstance();
    static void releaseInstance();

    // 初始化数据库（创建库+创建表）
    bool initDatabase();

    void writeDeviceLog(const QString &devName, const QString &operType, const QString &result, const QString &note = "");

    void writeDeviceState(const QString &devName, const QString &state);

    void writeSensorData(float temp, float humi);

private:

    DataBaseHelper();
    ~DataBaseHelper();
    DataBaseHelper(const DataBaseHelper&) = delete;
    DataBaseHelper& operator=(const DataBaseHelper&) = delete;

    static DataBaseHelper* m_instance; // 单例对象
    QSqlDatabase m_db;                 // 数据库连接对象
};

#endif // DATABASEHELPER_H
