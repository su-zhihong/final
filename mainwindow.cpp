#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_deviceModel(new DeviceModel(this))
    , m_tcpSocket(new QTcpSocket(this))
    , m_dataThread(new DataThread(this))
    , m_dbHelper(DataBaseHelper::getInstance())
    , m_chartXIndex(0)
{
    // 初始化数据库
    if(!m_dbHelper->initDatabase())
    {
        QMessageBox::critical(this, u8"错误", u8"数据库初始化失败！程序将退出");
        exit(1);
    }

    // 初始化UI+TCP+图表
    initUi();
    initTcpClient();
    initChart();

    // 启动后台线程：2秒采集一次传感器数据
    m_dataThread->startWork(2000);

    // 连接线程信号
    connect(m_dataThread, &DataThread::sigSensorDataUpdate, this, &MainWindow::slotSensorDataUpdate);
    connect(m_dataThread, &DataThread::sigHeartbeatStateUpdate, this, &MainWindow::slotHeartbeatStateUpdate);

    addLog(u8"智能家居控制中心启动成功！数据库连接正常");
}

MainWindow::~MainWindow()
{
    m_dataThread->stopWork();
    DataBaseHelper::releaseInstance();
}

void MainWindow::initUi()
{
    this->setWindowTitle(u8"智能家居控制中心");
    this->resize(1200, 800); // 主窗口大小

    // ========== 顶部工具栏 ==========
    QToolBar* toolBar = this->addToolBar(u8"场景模式");
    QPushButton* btnHomeMode = new QPushButton(u8"回家模式", this);
    QPushButton* btnAwayMode = new QPushButton(u8"离家模式", this);
    QPushButton* btnSleepMode = new QPushButton(u8"睡眠模式", this);
    toolBar->addWidget(btnHomeMode);
    toolBar->addWidget(btnAwayMode);
    toolBar->addWidget(btnSleepMode);
    // 绑定场景按钮槽函数
    connect(btnHomeMode, &QPushButton::clicked, this, &MainWindow::slotHomeModeClicked);
    connect(btnAwayMode, &QPushButton::clicked, this, &MainWindow::slotAwayModeClicked);
    connect(btnSleepMode, &QPushButton::clicked, this, &MainWindow::slotSleepModeClicked);

    // ========== 中心部件：左右分割 ==========
    QSplitter* mainSplitter = new QSplitter(Qt::Horizontal, this);
    mainSplitter->setSizes({280, 920});
    this->setCentralWidget(mainSplitter);

    // ========== 左侧：设备列表区 ==========
    m_widgetLeft = new QWidget(this);
    QVBoxLayout* vLayoutLeft = new QVBoxLayout(m_widgetLeft);
    vLayoutLeft->setSpacing(10);
    vLayoutLeft->setContentsMargins(10,10,10,10);
    mainSplitter->addWidget(m_widgetLeft);

    // 分组下拉框
    m_cbDeviceGroup = new QComboBox(this);
    m_cbDeviceGroup->addItems({u8"全部设备", u8"灯光设备", u8"空调设备", u8"门锁窗帘", u8"传感器设备"});
    vLayoutLeft->addWidget(m_cbDeviceGroup);
    connect(m_cbDeviceGroup, &QComboBox::currentIndexChanged, this, &MainWindow::slotFilterDeviceGroup);

    // 设备列表View
    m_viewDevice = new QTableView(this);
    m_viewDevice->setModel(m_deviceModel);
    m_viewDevice->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_viewDevice->setSelectionMode(QAbstractItemView::SingleSelection);
    m_viewDevice->resizeColumnsToContents();
    vLayoutLeft->addWidget(m_viewDevice);

    // 设备操作按钮组
    QWidget* btnWidget = new QWidget(this);
    QHBoxLayout* hLayoutBtn = new QHBoxLayout(btnWidget);
    m_btnOpenDev = new QPushButton(u8"开启设备", this);
    m_btnCloseDev = new QPushButton(u8"关闭设备", this);
    m_btnRefreshDev = new QPushButton(u8"刷新设备", this);
    hLayoutBtn->addWidget(m_btnOpenDev);
    hLayoutBtn->addWidget(m_btnCloseDev);
    hLayoutBtn->addWidget(m_btnRefreshDev);
    vLayoutLeft->addWidget(btnWidget);
    // 绑定按钮槽函数
    connect(m_btnOpenDev, &QPushButton::clicked, this, &MainWindow::slotOpenDevice);
    connect(m_btnCloseDev, &QPushButton::clicked, this, &MainWindow::slotCloseDevice);
    connect(m_btnRefreshDev, &QPushButton::clicked, this, &MainWindow::slotRefreshDevice);

    // ========== 右侧：数据展示区 ==========
    m_widgetRight = new QWidget(this);
    QVBoxLayout* vLayoutRight = new QVBoxLayout(m_widgetRight);
    vLayoutRight->setSpacing(10);
    vLayoutRight->setContentsMargins(10,10,10,10);
    mainSplitter->addWidget(m_widgetRight);

    // 传感器数据面板
    m_widgetSensor = new QGroupBox(u8"实时传感器数据", this);
    QHBoxLayout* hLayoutSensor = new QHBoxLayout(m_widgetSensor);
    m_lblTemp = new QLabel(u8"温度：-- ℃", this);
    m_lblHumi = new QLabel(u8"湿度：-- %", this);
    m_lblUpdateTime = new QLabel(u8"更新时间：--", this);
    hLayoutSensor->addWidget(m_lblTemp);
    hLayoutSensor->addWidget(m_lblHumi);
    hLayoutSensor->addStretch();
    hLayoutSensor->addWidget(m_lblUpdateTime);
    vLayoutRight->addWidget(m_widgetSensor);

    // 网络状态面板
    m_widgetNet = new QGroupBox(u8"网关通信状态", this);
    QHBoxLayout* hLayoutNet = new QHBoxLayout(m_widgetNet);
    m_lblNetState = new QLabel(u8"TCP状态：未连接", this);
    m_lblHeartbeat = new QLabel(u8"心跳状态：未检测", this);
    hLayoutNet->addWidget(m_lblNetState);
    hLayoutNet->addStretch();
    hLayoutNet->addWidget(m_lblHeartbeat);
    vLayoutRight->addWidget(m_widgetNet);

    // 图表展示区
    m_chartView = new QChartView(this);
    m_chartView->setMinimumHeight(200);
    vLayoutRight->addWidget(m_chartView);

    // 日志面板
    m_textLog = new QTextEdit(this);
    m_textLog->setReadOnly(true);
    m_textLog->setPlaceholderText(u8"系统日志将显示在这里...");
    vLayoutRight->addWidget(m_textLog);

    // ========== 状态栏 ==========
    this->statusBar()->showMessage(QString(u8"当前时间：%1 | 智能家居控制中心运行中").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")));
}

void MainWindow::initTcpClient()
{
    m_tcpSocket->connectToHost("127.0.0.1", 8888); // 模拟网关地址+端口
    connect(m_tcpSocket, &QTcpSocket::connected, this, &MainWindow::slotTcpConnected);
    connect(m_tcpSocket, &QTcpSocket::disconnected, this, &MainWindow::slotTcpDisconnected);
    connect(m_tcpSocket, &QTcpSocket::errorOccurred, this, &MainWindow::slotTcpError);
    connect(m_tcpSocket, &QTcpSocket::readyRead, this, &MainWindow::slotTcpReadyRead);
}

void MainWindow::initChart()
{
    m_chart = new QChart();
    m_chart->setTitle(u8"温湿度实时曲线");
    m_chart->legend()->setVisible(true);

    m_seriesTemp = new QLineSeries();
    m_seriesTemp->setName(u8"温度(℃)");
    m_seriesHumi = new QLineSeries();
    m_seriesHumi->setName(u8"湿度(%)");

    m_chart->addSeries(m_seriesTemp);
    m_chart->addSeries(m_seriesHumi);

    m_axisX = new QValueAxis();
    m_axisX->setTitleText(u8"采集次数");
    m_axisX->setRange(0, 20);
    m_axisY = new QValueAxis();
    m_axisY->setTitleText(u8"数值");
    m_axisY->setRange(18,70);

    m_chart->createDefaultAxes();
    m_chart->setAxisX(m_axisX, m_seriesTemp);
    m_chart->setAxisY(m_axisY, m_seriesTemp);
    m_chart->setAxisX(m_axisX, m_seriesHumi);
    m_chart->setAxisY(m_axisY, m_seriesHumi);

    m_chartView->setChart(m_chart);
    m_chartView->setRenderHint(QPainter::Antialiasing);
}

void MainWindow::addLog(const QString &logInfo)
{
    QString log = QString(u8"[%1] %2").arg(QDateTime::currentDateTime().toString("HH:mm:ss")).arg(logInfo);
    m_textLog->append(log);
}

void MainWindow::execHomeMode()
{
    addLog(u8"执行【回家模式】：开启所有灯光、客厅空调、打开窗帘，门锁保持解锁");
    m_deviceModel->updateDeviceState(u8"客厅主灯", u8"开启");
    m_deviceModel->updateDeviceState(u8"卧室台灯", u8"开启");
    m_deviceModel->updateDeviceState(u8"客厅空调", u8"开启");
    m_deviceModel->updateDeviceState(u8"客厅窗帘", u8"开启");
    m_deviceModel->updateDeviceState(u8"入户门锁", u8"已解锁");
    // 写入数据库
    m_dbHelper->writeDeviceLog(u8"系统", u8"场景模式", u8"执行成功", u8"回家模式");
}

void MainWindow::execAwayMode()
{
    addLog(u8"执行【离家模式】：关闭所有灯光、空调、窗帘，门锁自动上锁");
    m_deviceModel->updateDeviceState(u8"客厅主灯", u8"关闭");
    m_deviceModel->updateDeviceState(u8"卧室台灯", u8"关闭");
    m_deviceModel->updateDeviceState(u8"主卧空调", u8"关闭");
    m_deviceModel->updateDeviceState(u8"客厅空调", u8"关闭");
    m_deviceModel->updateDeviceState(u8"客厅窗帘", u8"关闭");
    m_deviceModel->updateDeviceState(u8"主卧窗帘", u8"关闭");
    m_deviceModel->updateDeviceState(u8"入户门锁", u8"已上锁");
    m_dbHelper->writeDeviceLog(u8"系统", u8"场景模式", u8"执行成功", u8"离家模式");
}

void MainWindow::execSleepMode()
{
    addLog(u8"执行【睡眠模式】：关闭所有灯光、客厅空调，关闭窗帘，门锁上锁，主卧空调低速运行");
    m_deviceModel->updateDeviceState(u8"客厅主灯", u8"关闭");
    m_deviceModel->updateDeviceState(u8"卧室台灯", u8"关闭");
    m_deviceModel->updateDeviceState(u8"客厅空调", u8"关闭");
    m_deviceModel->updateDeviceState(u8"主卧空调", u8"运行中");
    m_deviceModel->updateDeviceState(u8"客厅窗帘", u8"关闭");
    m_deviceModel->updateDeviceState(u8"主卧窗帘", u8"关闭");
    m_deviceModel->updateDeviceState(u8"入户门锁", u8"已上锁");
    m_dbHelper->writeDeviceLog(u8"系统", u8"场景模式", u8"执行成功", u8"睡眠模式");
}

void MainWindow::slotOpenDevice()
{
    QModelIndex index = m_viewDevice->currentIndex();
    if(!index.isValid()){ addLog(u8"提示：请选择要操作的设备"); return; }
    QString devName = m_deviceModel->data(m_deviceModel->index(index.row(), DeviceModel::Col_Name)).toString();
    QString devState = m_deviceModel->data(m_deviceModel->index(index.row(), DeviceModel::Col_State)).toString();
    if(devState == u8"开启" || devState == u8"已解锁" || devState == u8"运行中")
    {
        addLog(QString(u8"提示：设备【%1】已处于开启状态").arg(devName));
        return;
    }
    QString newState = (devName.contains(u8"门锁")) ? u8"已解锁" : (devName.contains(u8"传感器") ? u8"运行中" : u8"开启");
    m_deviceModel->updateDeviceState(devName, newState);
    addLog(QString(u8"成功开启设备：%1").arg(devName));
    m_dbHelper->writeDeviceLog(devName, u8"开启设备", u8"成功");
    m_dbHelper->writeDeviceState(devName, newState);
}

void MainWindow::slotCloseDevice()
{
    QModelIndex index = m_viewDevice->currentIndex();
    if(!index.isValid()){ addLog(u8"提示：请选择要操作的设备"); return; }
    QString devName = m_deviceModel->data(m_deviceModel->index(index.row(), DeviceModel::Col_Name)).toString();
    QString devState = m_deviceModel->data(m_deviceModel->index(index.row(), DeviceModel::Col_State)).toString();
    if(devState == u8"关闭" || devState == u8"已上锁")
    {
        addLog(QString(u8"提示：设备【%1】已处于关闭状态").arg(devName));
        return;
    }
    QString newState = (devName.contains(u8"门锁")) ? u8"已上锁" : u8"关闭";
    m_deviceModel->updateDeviceState(devName, newState);
    addLog(QString(u8"成功关闭设备：%1").arg(devName));
    m_dbHelper->writeDeviceLog(devName, u8"关闭设备", u8"成功");
    m_dbHelper->writeDeviceState(devName, newState);
}

void MainWindow::slotRefreshDevice()
{
    m_deviceModel->initDeviceData();
    m_cbDeviceGroup->setCurrentIndex(0);
    addLog(u8"设备列表已刷新");
    m_dbHelper->writeDeviceLog(u8"系统", u8"刷新设备", u8"成功");
}

void MainWindow::slotFilterDeviceGroup(int index)
{
    QString groupName = m_cbDeviceGroup->itemText(index);
    m_deviceModel->filterDeviceByGroup(groupName);
    addLog(QString(u8"筛选设备分组：%1").arg(groupName));
}

void MainWindow::slotSensorDataUpdate(float temp, float humi, QString time)
{
    m_lblTemp->setText(QString(u8"温度：%1 ℃").arg(temp, 0, 'f', 1));
    m_lblHumi->setText(QString(u8"湿度：%1 %").arg(humi, 0, 'f', 1));
    m_lblUpdateTime->setText(QString(u8"更新时间：%1").arg(time));
    // 更新图表
    m_seriesTemp->append(m_chartXIndex, temp);
    m_seriesHumi->append(m_chartXIndex, humi);
    m_chartXIndex++;
    if(m_chartXIndex > 20) m_axisX->setRange(m_chartXIndex-20, m_chartXIndex);
    // 写入数据库
    m_dbHelper->writeSensorData(temp, humi);
}

void MainWindow::slotHeartbeatStateUpdate(QString state, QString time)
{
    m_lblHeartbeat->setText(QString(u8"心跳状态：%1 %2").arg(state).arg(time));
}

void MainWindow::slotTcpConnected()
{
    m_lblNetState->setText(u8"TCP状态：已连接到网关");
    addLog(u8"TCP连接成功：已连接到家庭网关");
    m_dbHelper->writeDeviceLog(u8"网络", u8"TCP连接", u8"成功");
}

void MainWindow::slotTcpDisconnected()
{
    m_lblNetState->setText(u8"TCP状态：已断开连接");
    addLog(u8"TCP连接断开：与家庭网关断开连接");
    m_dbHelper->writeDeviceLog(u8"网络", u8"TCP断开", u8"成功");
}

void MainWindow::slotTcpError(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error)
    m_lblNetState->setText(QString(u8"TCP状态：连接失败 - %1").arg(m_tcpSocket->errorString()));
    addLog(QString(u8"TCP连接错误：%1").arg(m_tcpSocket->errorString()));
    m_dbHelper->writeDeviceLog(u8"网络", u8"TCP错误", u8"失败", m_tcpSocket->errorString());
}

void MainWindow::slotTcpReadyRead()
{
    QString recvData = m_tcpSocket->readAll();
    addLog(QString(u8"收到网关数据：%1").arg(recvData));
    m_dbHelper->writeDeviceLog(u8"网络", u8"接收数据", u8"成功", recvData);
}

void MainWindow::slotHomeModeClicked() { execHomeMode(); }
void MainWindow::slotAwayModeClicked() { execAwayMode(); }
void MainWindow::slotSleepModeClicked() { execSleepMode(); }
