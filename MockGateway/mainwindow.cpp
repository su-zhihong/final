#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    m_server = new QTcpServer(this);
    // 监听本地8888端口
    if(m_server->listen(QHostAddress::LocalHost, 8888))
    {
        connect(m_server, &QTcpServer::newConnection, this, &MainWindow::onNewConnection);
        this->setWindowTitle(u8"模拟家庭网关 - 8888端口已监听");
    }
    else
    {
        this->setWindowTitle(u8"网关启动失败");
    }
}

MainWindow::~MainWindow()
{
}

void MainWindow::onNewConnection()
{
    m_clientSocket = m_server->nextPendingConnection();
    connect(m_clientSocket, &QTcpSocket::readyRead, this, &MainWindow::onReadyRead);
    connect(m_clientSocket, &QTcpSocket::disconnected, this, &MainWindow::onClientDisconnected);
    // 给智能家居中心发送连接成功消息
    m_clientSocket->write(u8"网关连接成功，已就绪！");
}

void MainWindow::onReadyRead()
{

    QByteArray data = m_clientSocket->readAll();
    qDebug() << u8"收到控制中心指令：" << data;

    m_clientSocket->write(u8"指令已接收");
}

void MainWindow::onClientDisconnected()
{
    m_clientSocket->close();
}
