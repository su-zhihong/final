#ifndef DEVICEMODEL_H
#define DEVICEMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include <QVariant>
#include <QString>

// 设备结构体：存储设备所有信息
struct DeviceInfo
{
    QString devName;    // 设备名称
    QString devType;    // 设备类型：灯光/空调/门锁/窗帘/温湿度传感器
    QString devGroup;   // 设备分组：灯光设备/空调设备/门锁窗帘/传感器设备
    QString devState;   // 设备状态：开启/关闭/已上锁/已解锁/运行中
    QString devDesc;    // 设备描述
};

// 自定义TableModel，实现Model/View架构，负责设备数据的管理、分组过滤、数据更新
// 完全解耦：Model负责数据，View(QTableView)负责展示，Controller(主窗口)负责逻辑
class DeviceModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    // 列索引枚举，方便调用
    enum DeviceColumn
    {
        Col_Name = 0,
        Col_Type,
        Col_Group,
        Col_State,
        Col_Desc,
        Col_Count
    };

    explicit DeviceModel(QObject *parent = nullptr);

    // 重写Model核心虚函数
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // 自定义接口
    void initDeviceData(); // 初始化设备数据
    void updateDeviceState(const QString &devName, const QString &newState); // 更新设备状态
    void filterDeviceByGroup(const QString &groupName); // 按分组过滤设备

private:
    QList<DeviceInfo> m_allDeviceList;  // 所有设备数据
    QList<DeviceInfo> m_filterDeviceList; // 过滤后的设备数据
};

#endif // DEVICEMODEL_H
