#include "devicemodel.h"

DeviceModel::DeviceModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    initDeviceData();
}

int DeviceModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid()) return 0;
    return m_filterDeviceList.size();
}

int DeviceModel::columnCount(const QModelIndex &parent) const
{
    if(parent.isValid()) return 0;
    return Col_Count;
}

QVariant DeviceModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() || index.row() >= m_filterDeviceList.size() || index.column() >= Col_Count)
        return QVariant();

    const DeviceInfo &dev = m_filterDeviceList.at(index.row());
    if(role == Qt::DisplayRole)
    {
        switch (index.column())
        {
        case Col_Name: return dev.devName;
        case Col_Type: return dev.devType;
        case Col_Group: return dev.devGroup;
        case Col_State: return dev.devState;
        case Col_Desc: return dev.devDesc;
        default: break;
        }
    }
    return QVariant();
}

QVariant DeviceModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole) return QVariant();
    if(orientation == Qt::Horizontal)
    {
        switch (section)
        {
        case Col_Name: return u8"设备名称";
        case Col_Type: return u8"设备类型";
        case Col_Group: return u8"设备分组";
        case Col_State: return u8"设备状态";
        case Col_Desc: return u8"设备描述";
        default: break;
        }
    }
    return QVariant();
}

void DeviceModel::initDeviceData()
{
    beginResetModel();

    if(m_allDeviceList.isEmpty())
    {
        DeviceInfo dev1{u8"客厅主灯", u8"灯光", u8"灯光设备", u8"关闭", u8"客厅主照明"};
        DeviceInfo dev2{u8"卧室台灯", u8"灯光", u8"灯光设备", u8"关闭", u8"卧室辅助照明"};
        DeviceInfo dev3{u8"主卧空调", u8"空调", u8"空调设备", u8"关闭", u8"主卧温度调节"};
        DeviceInfo dev4{u8"客厅空调", u8"空调", u8"空调设备", u8"关闭", u8"客厅温度调节"};
        DeviceInfo dev5{u8"入户门锁", u8"门锁", u8"门锁窗帘", u8"已上锁", u8"大门智能锁"};
        DeviceInfo dev6{u8"客厅窗帘", u8"窗帘", u8"门锁窗帘", u8"关闭", u8"客厅电动窗帘"};
        DeviceInfo dev7{u8"主卧窗帘", u8"窗帘", u8"门锁窗帘", u8"关闭", u8"主卧电动窗帘"};
        DeviceInfo dev8{u8"温湿度传感器", u8"传感器", u8"传感器设备", u8"运行中", u8"全屋环境监测"};

        m_allDeviceList.append(dev1);
        m_allDeviceList.append(dev2);
        m_allDeviceList.append(dev3);
        m_allDeviceList.append(dev4);
        m_allDeviceList.append(dev5);
        m_allDeviceList.append(dev6);
        m_allDeviceList.append(dev7);
        m_allDeviceList.append(dev8);
    }
    // 刷新过滤列表，保持当前的分组筛选状态
    m_filterDeviceList = m_allDeviceList;
    endResetModel();
}

void DeviceModel::updateDeviceState(const QString &devName, const QString &newState)
{

    for(int i=0; i<m_allDeviceList.size(); i++)
    {
        if(m_allDeviceList[i].devName == devName)
        {
            m_allDeviceList[i].devState = newState;
            break;
        }
    }


    for(int i=0; i<m_filterDeviceList.size(); i++)
    {
        if(m_filterDeviceList[i].devName == devName)
        {
            m_filterDeviceList[i].devState = newState;

            QModelIndex index = createIndex(i, Col_State);
            emit dataChanged(index, index);
            break;
        }
    }
}

void DeviceModel::filterDeviceByGroup(const QString &groupName)
{
    beginResetModel();
    if(groupName == u8"全部设备")
        m_filterDeviceList = m_allDeviceList;
    else
    {
        m_filterDeviceList.clear();
        for(const auto &dev : m_allDeviceList)
        {
            if(dev.devGroup == groupName)
                m_filterDeviceList.append(dev);
        }
    }
    endResetModel();
}
