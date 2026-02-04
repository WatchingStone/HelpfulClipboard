// 【侧边栏】类，处理纯粹的 UI 渲染逻辑
#ifndef SIDEBARWIDGET_H
#define SIDEBARWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QVBoxLayout>
#include <QSharedPointer>
#include "DataStruct.h"
#include <QCheckBox>
#include <QPushButton>
#include <QPoint>


enum ItemDataAttribute{     // widget中的 QListWidget 中的 QListWidgetItem 的自定义属性，我专门设置的其属性枚举
    Dataid = Qt::UserRole,
    Datatype,
    imgData,
    textData,
    urlData,
    otherData
};

class SidebarWidget : public QWidget{
    Q_OBJECT
    private:
        QListWidget *listWidget;        // 侧边栏的ui显示列表
        QCheckBox *checkBox;            // 框选按钮，表示是否开启【常驻保持侧边栏】模式
        QWidget *headlineWidget;        // 侧边栏的标题行
        QHBoxLayout *headlineLayout;    
        QPushButton *closeWindowBtn;    // 窗口关闭按钮，可以直接关闭侧边栏
        QVBoxLayout *mainLayout;        // 侧边栏的主布局
        QPoint press_start_position;    // 拖拽判断的参考点
        QPoint drag_position;           // 拖拽判断的参考点
    
    public:
        explicit SidebarWidget(QWidget *parent = nullptr);
        void updateList(const QList<QSharedPointer<CopyableData>> &history);
        bool eventFilter(QObject *obj, QEvent *event) override; // 对于下属子组件设置的过滤器，指向子组件的事件会首先经过本父组件进行处理
    
    signals:
        void _itemSelected(QSharedPointer<CopyableData> data);  // 当列表中的元素被点击时，发出信号
        void _keepSidebarBtnPressed(bool checked);              // 当侧边栏中的【保持侧边栏】按钮被选中/取消时，发出信号
    
    // protected:
    //     void mousePressEvent(QMouseEvent *event) override;      // 为了实现拖拽侧边栏，重写鼠标移动事件
    //     void mouseMoveEvent(QMouseEvent *event) override;
        // void mouseReleaseEvent(QMouseEvent *event) override;
};


#endif // SIDEBARWIDGET_H