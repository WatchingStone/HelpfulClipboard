// QuickSlot快捷栏的槽位，需要重写鼠标拖拽相关事件

#ifndef QUICKSLOT_H
#define QUICKSLOT_H

#include <QWidget>
#include <QSharedPointer>
#include <QLabel>
#include "SlotUIWidget.h"
#include "DataStruct.h"
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QMouseEvent>

class QuickSlot : public QWidget{
    Q_OBJECT
    private:
        int id;                             // 槽位id
        QSharedPointer<CopyableData> data;  // 槽位数据
        SlotUIWidget *ui;                   // 槽位UI
        QuickSlot *clickStartFrom;          // 对于点击事件

    
    public:
        explicit QuickSlot(int id, QWidget *parent = nullptr);  // 构造函数需要传入槽位id
        void setData(QSharedPointer<CopyableData> data);        // 设置槽位数据
        QSharedPointer<CopyableData> getData();
        void refreshUI();

    signals:
        void _quickslotChanged(int id, QSharedPointer<CopyableData> data);  // 当槽位内数据发生变化时
        void _quickslotClicked(int id, QSharedPointer<CopyableData> data);          // 当槽位被点击时
    
    protected:
        // 重写拖拽（进入）事件
        void dragEnterEvent(QDragEnterEvent *event) override;  
        void dropEvent(QDropEvent *event) override;
        // 重写鼠标点击事件
        void mousePressEvent(QMouseEvent *event) override;
        void mouseReleaseEvent(QMouseEvent *event) override;
};

#endif // QUICKSLOT_H