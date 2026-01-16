#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableView>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QTcpSocket>
#include <QDateTime>
#include <QMessageBox>
#include <QChart>
#include <QLineSeries>
#include <QValueAxis>
#include <QChartView>
#include <QPainter>
#include <QToolBar>      //✅ 补充缺失头文件
#include <QStatusBar>    //✅ 补充缺失头文件
#include <QAbstractSocket>

#include "devicemodel.h"
#include "datathread.h"
#include "databasehelper.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    // 初始化UI界面+控件命名+布局（核心，严格按你的要求）
    void initUi();
    // 初始化TCP网络连接（模拟网关通信）
    void initTcpClient();
    // 初始化图表（CHART可选模块）
    void initChart();
    // 添加日志到日志面板
    void addLog(const QString &logInfo);
    // 场景模式执行函数
    void execHomeMode();   // 回家模式
    void execAwayMode();   // 离家模式
    void execSleepMode();  // 睡眠模式

private slots:
    // 设备操作槽函数
    void slotOpenDevice();
    void slotCloseDevice();
    void slotRefreshDevice();
    void slotFilterDeviceGroup(int index);
    // 传感器数据更新槽函数
    void slotSensorDataUpdate(float temp, float humi, QString time);
    // 心跳包状态更新槽函数
    void slotHeartbeatStateUpdate(QString state, QString time);
    // TCP网络状态槽函数
    void slotTcpConnected();
    void slotTcpDisconnected();
    void slotTcpError(QAbstractSocket::SocketError error);
    void slotTcpReadyRead();
    // 场景模式按钮槽函数
    void slotHomeModeClicked();
    void slotAwayModeClicked();
    void slotSleepModeClicked();

private:
    // ========== Model/View 核心控件 ==========
    DeviceModel* m_deviceModel;
    QTableView* m_viewDevice;

    // ========== 左侧控件 ==========
    QWidget* m_widgetLeft;
    QComboBox* m_cbDeviceGroup;
    QPushButton* m_btnOpenDev;
    QPushButton* m_btnCloseDev;
    QPushButton* m_btnRefreshDev;

    // ========== 右侧控件 ==========
    QWidget* m_widgetRight;
    QGroupBox* m_widgetSensor;
    QLabel* m_lblTemp;
    QLabel* m_lblHumi;
    QLabel* m_lblUpdateTime;
    QGroupBox* m_widgetNet;
    QLabel* m_lblNetState;
    QLabel* m_lblHeartbeat;
    QTextEdit* m_textLog;

    // ========== 图表控件（CHART） ==========
    QChart* m_chart;
    QLineSeries* m_seriesTemp;
    QLineSeries* m_seriesHumi;
    QChartView* m_chartView;
    QValueAxis* m_axisX;
    QValueAxis* m_axisY;
    int m_chartXIndex;


    QTcpSocket* m_tcpSocket;       // TCP通信（与网关）
    DataThread* m_dataThread;      // 后台数据线程
    DataBaseHelper* m_dbHelper;    // 数据库工具类
};

#endif // MAINWINDOW_H
