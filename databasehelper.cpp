#include "databasehelper.h"

DataBaseHelper* DataBaseHelper::m_instance = nullptr;

DataBaseHelper::DataBaseHelper()
{

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("SmartHomeDB.db");
}

DataBaseHelper::~DataBaseHelper()
{
    if(m_db.isOpen())
        m_db.close();
}

DataBaseHelper *DataBaseHelper::getInstance()
{
    if(m_instance == nullptr)
        m_instance = new DataBaseHelper;
    return m_instance;
}

void DataBaseHelper::releaseInstance()
{
    if(m_instance != nullptr)
    {
        delete m_instance;
        m_instance = nullptr;
    }
}

bool DataBaseHelper::initDatabase()
{
    if(!m_db.open())
    {
        qDebug() << "数据库打开失败：" << m_db.lastError().text();
        return false;
    }

    // 创建设备日志表
    QSqlQuery queryLog;
    QString sqlLog = "CREATE TABLE IF NOT EXISTS DeviceLog("
                     "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                     "devName TEXT NOT NULL,"
                     "operType TEXT NOT NULL,"
                     "result TEXT NOT NULL,"
                     "note TEXT,"
                     "createTime DATETIME DEFAULT CURRENT_TIMESTAMP)";
    if(!queryLog.exec(sqlLog))
    {
        qDebug() << "创建日志表失败：" << queryLog.lastError().text();
        return false;
    }

    // 创建设备状态表
    QSqlQuery queryState;
    QString sqlState = "CREATE TABLE IF NOT EXISTS DeviceState("
                       "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                       "devName TEXT UNIQUE NOT NULL,"
                       "state TEXT NOT NULL,"
                       "updateTime DATETIME DEFAULT CURRENT_TIMESTAMP)";
    if(!queryState.exec(sqlState))
    {
        qDebug() << "创建状态表失败：" << queryState.lastError().text();
        return false;
    }

    // 创建传感器数据表
    QSqlQuery querySensor;
    QString sqlSensor = "CREATE TABLE IF NOT EXISTS SensorData("
                        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                        "temperature REAL NOT NULL,"
                        "humidity REAL NOT NULL,"
                        "collectTime DATETIME DEFAULT CURRENT_TIMESTAMP)";
    if(!querySensor.exec(sqlSensor))
    {
        qDebug() << "创建传感器表失败：" << querySensor.lastError().text();
        return false;
    }

    qDebug() << "数据库初始化成功！";
    return true;
}

void DataBaseHelper::writeDeviceLog(const QString &devName, const QString &operType, const QString &result, const QString &note)
{
    QSqlQuery query;
    query.prepare("INSERT INTO DeviceLog(devName, operType, result, note) VALUES(:devName, :operType, :result, :note)");
    query.bindValue(":devName", devName);
    query.bindValue(":operType", operType);
    query.bindValue(":result", result);
    query.bindValue(":note", note);
    if(!query.exec())
        qDebug() << "写入日志失败：" << query.lastError().text();
}

void DataBaseHelper::writeDeviceState(const QString &devName, const QString &state)
{
    QSqlQuery query;
    // 存在则更新，不存在则插入
    query.prepare("INSERT OR REPLACE INTO DeviceState(devName, state) VALUES(:devName, :state)");
    query.bindValue(":devName", devName);
    query.bindValue(":state", state);
    if(!query.exec())
        qDebug() << "写入状态失败：" << query.lastError().text();
}

void DataBaseHelper::writeSensorData(float temp, float humi)
{
    QSqlQuery query;
    query.prepare("INSERT INTO SensorData(temperature, humidity) VALUES(:temp, :humi)");
    query.bindValue(":temp", temp);
    query.bindValue(":humi", humi);
    if(!query.exec())
        qDebug() << "写入传感器数据失败：" << query.lastError().text();
}
