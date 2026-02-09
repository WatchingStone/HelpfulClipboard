#include <windows.h>
#include "Quickbar.h"
#include <QWidget>
#include <QScreen>
#include <QGuiApplication>
#include <QFile>
#include <QStandardPaths>
#include <QDir>
#include <QDataStream>
// #include <QString>
#include <QDebug>


Quickbar::Quickbar(QWidget *parent)
    : QWidget(parent){
    /*构造函数*/ 
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool | Qt::WindowDoesNotAcceptFocus);
    // 使用windows的原生补丁，强行设置窗口属性
    #ifdef Q_OS_WIN
    HWND hwnd = (HWND)this->winId();                                // 获取本窗口的句柄
    LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
    SetWindowLong(hwnd, GWL_EXSTYLE, exStyle | WS_EX_NOACTIVATE);   // 即使用户点击了该窗口，也不尝试传递焦点给它
    #endif  // Q_OS_WIN

    quickbarLayout = new QVBoxLayout(this);

    // 循环创建1-9快捷栏槽位
    for(int i = 0; i < 9; ++i){
        QuickSlot *slot = new QuickSlot(i, this);
        quickbarLayout->addWidget(slot);
        slotlist.append(slot);

        // 每个都绑定信号
        connect(slot, &QuickSlot::_quickslotClicked, this, &Quickbar::_on_quickbar_get_slot_clicked_DO_send_to_upper);
    }

    load();     // 从存储文件加载快捷栏内容

    // 在加载完存储文件后再绑定信号，防止循环触发
    for(QuickSlot *slot : slotlist){
        connect(slot, &QuickSlot::_quickslotChanged, this, &Quickbar::save);        // 每次槽位内容变化时就保存一次
    }
    
}

QSharedPointer<CopyableData> Quickbar::getDataInSlot(int id){
    /*需要提供一个接口给 MainController，用来根据索引（1-9）获取数据*/
    return slotlist.at(id)->getData();
}


void Quickbar::adjustPositonToScreenEdge(){
    /*将快捷栏移动到主屏幕右边缘，调用时机不是Quickbar的构造函数内，而是交给MainController的.start()去调用*/
    QScreen *screen = QGuiApplication::primaryScreen(); // 获取主屏幕对象
    if(!screen) return;

    // 刷新快捷栏窗口大小，因为其大小是被QuickSlot撑开的，所以要让系统更新出真实的尺寸
    this->adjustSize();

    // 计算目标位置
    QRect availabelRect = screen->availableGeometry();  // 获取屏幕可用区域（即，除任务栏外的区域）
    int x = availabelRect.right() - this->width();  
    int y = availabelRect.top() + (availabelRect.height() - this->height()) / 2;    // 高度居中
    this->move(x, y);
}

void Quickbar::_on_quickbar_get_slot_clicked_DO_send_to_upper(int id, QSharedPointer<CopyableData> data){
    /*当快捷栏下属槽位发出其被点击的信号时，向 MainContorller 发出信号*/
    emit _quickbarGetSlotClicked(data);
}


QString Quickbar::getSavePath(){
    /*获取快捷栏内容保存路径，返回程序所在目录下的文件保存路径*/
    return QCoreApplication::applicationDirPath() + "/quickbar.bat";
}

void Quickbar::save(){
    /*保存快捷栏内容*/
    qDebug() << ">> 【Quickbar】正在保存";

    QFile file(getSavePath());
    if(!file.open(QIODevice::WriteOnly)){
        qDebug() << ">> 【Quickbar】无法打开文件进行保存";
        return ;
    }
    QDataStream out(&file);
    int version = 100;
    out << version;                 // 写入版本号，养成好习惯，方便以后升级格式

    // 遍历每一个槽位进行保存
    for(QuickSlot *slot : slotlist){
        // 从槽位读取数据
        QSharedPointer<CopyableData> data = slot->getData();

        // 用一个bool表示该槽位是否为空
        bool hasData = (data != nullptr);   
        out << hasData;
        if(hasData){
            out << *data;           // 利用 DataStruct 重载的流数据和序列化实现
        }
    }
    qDebug() << ">> 【Quickbar】保存已完成";
}
void Quickbar::load(){
    /*从保存路径读取保存文件，加载进快捷栏*/
    qDebug() << ">> 【Quickbar】正在读取";

    QFile file(getSavePath());

    // 检查文件可读性
    if(!file.exists()){
        qDebug() << ">> 【Quickbar】目标路径文件不存在！";
        return;
    }
    if(!file.open(QIODevice::ReadOnly)){
        qDebug() << ">> 【Quickbar】目标路径文件无法打开！";
        return;
    }

    // 以二进制数据流形式读取文件
    QDataStream in(&file);
    int version;
    in >> version;          // 先读取版本号

    for(int i = 0; i < slotlist.size(); ++i){
        bool hasData;
        in >> hasData;      // 检查该槽位是否有存储记录
        if(hasData){
            QSharedPointer<CopyableData> newData(new CopyableData());
            in >> *newData; // 反序列化
            slotlist[i]->setData(newData);
        }
        // slotlist[i]->refreshUI();   // 刷新显示
    }

    qDebug() << ">> 【Quickbar】完成从存储文件加载快捷栏";
}