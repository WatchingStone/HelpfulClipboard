
#include <QByteArray>
#include <QDataStream>
#include "QuickSlot.h"
#include <QDebug>

QuickSlot::QuickSlot(int id, QWidget *parent)
    :id(id), QWidget(parent){
    // 增加一个明显的背景或边框，让用户能看到每个槽位在哪里
    this->setStyleSheet("QuickSlot { border: 1px dashed #aaa; background-color: rgba(255, 255, 255, 50); }");

    // 允许接受掉落，作为拖拽的终点
    setAcceptDrops(true); 

    ui = new SlotUIWidget(id, nullptr, this);

    QVBoxLayout *slotlayout = new QVBoxLayout(this);
    slotlayout->setContentsMargins(0, 0, 0, 0); 
    slotlayout->addWidget(ui);
    
}

void QuickSlot::setData(QSharedPointer<CopyableData> d){
    /*更新slot内的数据，刷新显示*/
    data = d;
    if(ui){
        ui->setData(data);              // 刷新ui显示
    }
    emit _quickslotChanged(id, data);   // 发送slot数据变更信号
}

QSharedPointer<CopyableData> QuickSlot::getData(){
    return data;
}

void QuickSlot::refreshUI(){
    if(ui) ui->refreshUI();             // 刷新ui显示
}

void QuickSlot::mousePressEvent(QMouseEvent *event){
    /*鼠标点击事件*/
    qDebug() << ">> 【QuickSlot】-[" << QString::number(id) << "]发生【鼠标按下】事件";
}
void QuickSlot::mouseReleaseEvent(QMouseEvent *event){
    /*鼠标释放事件*/
    qDebug() << ">> 【QuickSlot】-[" << QString::number(id) << "]发生【鼠标释放】事件";
    if(event->button() == Qt::LeftButton){
        if(data) emit _quickslotClicked(id, data);  // 只有当是鼠标左键释放时，当本slot内有数据时，才发出信号
        qDebug() << ">> 【QuickSlot】-[" << QString::number(id) << "]发出信号::_quickSlotClicked(QSharedPointer<CopyableData> data)";
    }
}

void QuickSlot::dragEnterEvent(QDragEnterEvent *event){
    /*重写拖拽进入事件，检查进入数据的类型是否正确*/
    if(event->mimeData()->hasFormat(MIME_TYPE_COPYABLE_DATA)){  // 只接受来自Sidebar的listWidget发送来的特定格式数据
        event->acceptProposedAction();                          // 接受该拖拽的进入
    }
}
void QuickSlot::dropEvent(QDropEvent *event){
    /*重写拖拽放下事件*/

    // // 提取还原数据
    // QByteArray data_address = event->mimeData()->data(MIME_TYPE_COPYABLE_DATA);
    // quintptr temp_data_address = data_address.toULongLong();
    // CopyableData *temp_data = reinterpret_cast<CopyableData*>(temp_data_address);
    // QSharedPointer<CopyableData> data_ptr(temp_data);

    // 提取还原数据
    QByteArray item_address = event->mimeData()->data(MIME_TYPE_COPYABLE_DATA);
    QDataStream data_stream(&item_address, QIODevice::ReadOnly);

    // 创建一个全新的对象作为副本，来读数据，这个 newData 拥有独立的内存和生命周期
    QSharedPointer<CopyableData> newData(new CopyableData());
    data_stream >> *newData;    // 按照 DataStruct 重载的流操作和反序列化自动实现
    setData(newData);           // 将数据存入本slot
    event->acceptProposedAction();
}