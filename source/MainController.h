// 主程序，下属包含【悬浮球】和【侧边栏】类

#ifdef Q_OS_WIN
#include <windows.h>    // 用来处理事件焦点、模拟键盘按键事件
#endif

#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <QObject>
#include <QSharedPointer>
#include "ClipboardListener.h"
#include "ClipboardManager.h"
#include "FloatingBall.h"
#include "SidebarWidget.h"



class MainController : public QObject{
    Q_OBJECT
    private:
        ClipboardListener *cb_listener; // 剪贴板监听器
        ClipboardManager *cb_manager;   // 剪贴板历史记录条目管理器（核心的数据存储结构）
        FloatingBall *ball;             // 悬浮球
        SidebarWidget *sidebar;         // 侧边栏
        bool keep_sidebar_top_hint;     // 在点击侧边栏条目并完成写回后是否保持显示侧边栏


        void restoreFocusAndPaste();    // 恢复至上一个窗口焦点，然后粘贴
        
        // void write_to_clipboard(QSharedPointer<CopyableData> d);
        // void simulate_Press_Ctrl_V();   // 模拟按下ctrl+v事件
        // void refreshUI();               // 更新UI显示
        // void showSideBarAtBall();       // 进行悬浮球到侧边栏的转换，在悬浮球的坐标处显示侧边栏

    public:
        explicit MainController(QObject *parent = nullptr);
        void start();

    signals:
        // void _on_get_clipboard_data(QSharedPointer<CopyableData>);

    private slots:
        void _on_ball_get_left_clicked();                                       // 当悬浮球被左键点击时，进行
        void _on_ball_get_right_clicked();                                      // 当悬浮球被右键点击时，进行
        void _on_sidebar_item_get_selected(QSharedPointer<CopyableData> data);  // 当侧边栏的列表元素被选中时，进行
        void _on_keep_sidebar_BTN_pressd_DO_set_flag(bool status);              // 当侧边栏的【保持显示】按钮被选中或取消时，进行
        
        // 核心：写入剪贴板并粘贴（如果是从侧边栏唤起的写入操作，则需要闪烁隐藏侧边栏）
        void write_and_paste(QSharedPointer<CopyableData> data);
        
        /*
        我固定设置命名规则：
            - 信号声明：           “下划线_”开头，首个单词不能是“on”，此后驼峰命名
            - 槽（处理函数）命名：   “下划线_”开头，首个单词是“on”，此后蛇形命名。同时，如果有必要，则前半部分描述接收信号名，后半部分描述改函数的功能，中间以“_DO_”间隔
        */
};
#endif // MAINCONTROLLER_H