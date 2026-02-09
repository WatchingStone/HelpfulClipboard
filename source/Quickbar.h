// Quickbar快捷栏类，只管理ui
/*
1. 常驻于屏幕右侧，紧贴屏幕右边缘
2. 内容是“快捷键槽位”，即，常驻了数字1-9个槽位，默认是空的。
3. 当用户在侧边栏中用鼠标左键拖拽条目到快捷栏中的槽位时，在快捷栏的对应槽位中记录该数据
4. 当程序关闭后，该槽位的数据将被持久化存储，下次启动程序时，侧边栏的历史条目会被清空重启，而快捷栏记录的数据会被恢复
5. 用户可以用“鼠标左键点击”的方式点击快捷栏内的槽位来插入，也可以通过“ctrl+数字键”的快捷键组合便捷插入
*/

#ifndef QUICKBAR_H
#define QUICKBAR_H

#include <QWidget>
#include <QVBoxLayout>
#include <QList>
#include "QuickSlot.h"
#include <QString>

class Quickbar : public QWidget{
    Q_OBJECT
    private:
        QVBoxLayout *quickbarLayout;
        QList<QuickSlot*> slotlist;         // 快捷栏槽位（默认1-9）

        QString getSavePath();              // 获取快捷栏内容保存路径
        void save();                        // 保存快捷栏内容
        void load();                        // 从保存路径读取保存文件，加载进快捷栏
    
    public:
        explicit Quickbar(QWidget *parent = nullptr);
        QSharedPointer<CopyableData> getDataInSlot(int id); // 需要提供一个接口给 MainController，用来根据索引（1-9）获取数据
        void adjustPositonToScreenEdge();   // 将快捷栏移到屏幕右边缘
    
    signals:
        void _quickbarGetSlotClicked(QSharedPointer<CopyableData> data);    // 向MainContorller发出信号
    
    private slots:
        void _on_quickbar_get_slot_clicked_DO_send_to_upper(int id, QSharedPointer<CopyableData> data); // 当下属槽位发出其被点击的信号时，向MainContorller发出信号
};  

#endif // QUICKBAR_H