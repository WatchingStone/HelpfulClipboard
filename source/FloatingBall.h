// 悬浮球类，实现显示单条数据，绘制圆形UI，将成为主程序类的核心私有成员变量之一
// 监听鼠标的拖拽、左键单击、右键单击
#ifndef FLOATINGBALL_H
#define FLOATINGBALL_H

#include <QWidget>
#include "DataStruct.h"

class FloatingBall : public QWidget{
    Q_OBJECT

    private:
        QSharedPointer<CopyableData> currentData = nullptr; // 当前悬浮球显示的条目
        QPoint drag_position;                               // 鼠标进行拖拽事件时有坐标移动
        QPoint press_start_position;                        // 鼠标点击时的坐标，用来区分点击和拖拽
        bool is_dragging = false;                           // 区分点击和拖拽事件

    public:
        explicit FloatingBall(QWidget *parent = nullptr);
        void refreshUI(QSharedPointer<CopyableData> data);  // 传入1个新数据，刷新当前的显示
    
    signals:
        void _leftClicked();                                // 信号：悬浮球被左键点击（不考虑双击
        void _rightClicked();                               // 信号：悬浮球被右键点击

    protected:
        // 重写4个事件
        // 这些函数是事件处理器，在父类QWidget中就是virtual protected，所以以protected状态重写它们
        void paintEvent(QPaintEvent *event) override;
        void mousePressEvent(QMouseEvent *event) override;
        void mouseMoveEvent(QMouseEvent *event) override;
        void mouseReleaseEvent(QMouseEvent *event) override;
};

#endif // FLOATINGBALL_H

// Qt的绘制：
// 在 Qt 中，QWidget 就像一块黑板。你不能随时随地在上面写字。系统有一套自己的调度逻辑，只有当系统告诉你“现在是绘画时间”时，你才能拿起粉笔。
// 规则： QPainter 必须（几乎 99% 的情况下）声明在 paintEvent 函数内部，作为一个局部变量。
// 当 paintEvent 结束时，QPainter 对象被销毁，绘画动作才会真正提交给屏幕显卡