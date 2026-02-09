#include "MainController.h"
#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QTimer>
#include "WindowsHelper.h"
#include <QCheckBox>
#include <QDebug>

MainController::MainController(QObject *parent) : QObject(parent) {
    // 注册自定义类型
    qRegisterMetaType<QSharedPointer<CopyableData>>("QSharedPointer<CopyableData>");

    // 初始化所有组件
    cb_listener = new ClipboardListener(this);
    cb_manager  = new ClipboardManager(5, this);
    ball     = new FloatingBall();
    sidebar  = new Sidebar();
    quickbar = new Quickbar();
    kb_listener = new KeyboardListener();

    // 连接信号槽
    /*当【监听器】发出“获得新数据”信号时，交给【管理器】，添加进历史记录*/
    connect(cb_listener, &ClipboardListener::_getClipboardData, cb_manager, &ClipboardManager::_on_get_new_data_DO_add_data);
    /*当【管理器】发出“历史记录变更”信号时，本MainController控制组件进行渲染更新*/
    connect(cb_manager, &ClipboardManager::_historyChanged, this, [this](){
        sidebar->updateList(cb_manager->getHistory());  // 从管理器获取当前的历史条目列表，交给侧边栏进行渲染
    });
    /*当【管理器】发出“最新条目变更”信号时，交给【悬浮球】，刷新UI*/
    connect(cb_manager, &ClipboardManager::_lastestDataChanged, ball, &FloatingBall::refreshUI);

    // 悬浮球的信号
    connect(ball, &FloatingBall::_leftClicked, this, &MainController::_on_ball_get_left_clicked);
    connect(ball, &FloatingBall::_rightClicked, this, &MainController::_on_ball_get_right_clicked);

    // 侧边栏的信号
    connect(sidebar, &Sidebar::_itemSelected, this, &MainController::_on_sidebar_item_get_selected);
    connect(sidebar, &Sidebar::_keepSidebarBtnPressed, this, [this](bool checked){
        if(checked) _on_keep_sidebar_BTN_pressd_DO_set_flag(true);
        else _on_keep_sidebar_BTN_pressd_DO_set_flag(false);
    });

    // 快捷栏的信号
    connect(quickbar, &Quickbar::_quickbarGetSlotClicked, this, &MainController::write_and_paste);

    // 键盘监听器的信号
    connect(kb_listener, &KeyboardListener::_slotHotkeyPressed, this, [this](int slotId){
        /*当键盘监听器监听到热键触发，则直接从快捷栏对应槽位中读取数据进行回写*/
        qDebug() << ">>>【MainController】接收来自【KeyboardListener】的热键触发信号：slotId = " << slotId;

        QSharedPointer<CopyableData> data = quickbar->getDataInSlot(slotId);
        if(data){
            qDebug() << ">>>【MainController】正在回写【Quickbar】下【QuickSlot = "<< slotId << "】"; 
            write_and_paste(data);
        }
        else{
            qDebug() << ">>>【MainController】在【Quickbar】的.getDataInSlot(slotId = " << slotId << ") 的内容为空"; 
        }
    });
    
    qDebug() << ">>>【MainContorller】完成构造函数";
}

void MainController::start(){
    /*程序初始化状态*/
    ball->show();
    quickbar->show();
    quickbar->adjustPositonToScreenEdge();
    qDebug() << ">>>【MainContorller】完成.start()";
}

void MainController::_on_ball_get_left_clicked(){
    /*当悬浮球被左键点击时，读取最新的历史记录条目，写回剪贴板并粘贴*/
    QSharedPointer<CopyableData> data = cb_manager->getLatest();
    if(data){
        qDebug() << ">>>【MainController】正在处理【悬浮球被左键点击】事件";
        write_and_paste(data);
    }
}

void MainController::_on_ball_get_right_clicked(){
    /*当悬浮球被右键点击时，显示侧边栏（即时计算坐标）*/
    QPoint ball_position = ball->geometry().topLeft();
    sidebar->move(ball_position.x() - sidebar->width(), ball_position.y());
    sidebar->show();                // 显示侧边栏
    sidebar->activateWindow();      // 获取焦点
}

void MainController::_on_sidebar_item_get_selected(QSharedPointer<CopyableData> data){
    /*当侧边栏内的元素被点击时，进行回写操作*/
    write_and_paste(data);
    if(!keep_sidebar_top_hint) sidebar->hide();     // 如果不是常驻侧边栏模式，则每点击一次侧边栏都直接隐藏
}

void MainController::_on_keep_sidebar_BTN_pressd_DO_set_flag(bool status){
    /*当侧边栏的【保持显示】按钮被选中或取消时，变更标志位*/
    keep_sidebar_top_hint = status;
}



void MainController::write_and_paste(QSharedPointer<CopyableData> d){
    /*处理回写函数，将CopyableData数据转换回mimedata*/
    qDebug() << ">>>【MainController】尝试回写剪贴板并粘贴";

    if(!d) return;
    cb_listener->setEnable(false);      // 临时禁用剪贴板监听器，防止反复触发剪贴板变更事件

    // 写回系统剪贴板
    qDebug() << ">>>【MainController】正在写入剪贴板";
    QClipboard *clip = QApplication::clipboard();   // 创建全局唯一的剪贴板对象作为句柄
    switch(d->getType()){
        case DataType::text:
            clip->setText(d->getText());
            break;
        case DataType::img:
            clip->setImage(d->getImage());
            break;
        case DataType::urls:
            // 对于urls格式的数据，最好保留urls和text属性，所以使用完整的QMimedata对象
            QMimeData *mimedata = new QMimeData();
            mimedata->setUrls(d->getUrls());
            mimedata->setText(d->getText());
            clip->setMimeData(mimedata);            // .setMimeData()会接管mimedata的数据，无需额外考虑资源释放
            break;
    }
    // 完成写回
    qDebug() << ">>>【MainController】完成回写剪贴板";

    QTimer::singleShot(100, [this](){
        // 进行粘贴
            restoreFocusAndPaste();

            // 延迟300ms再恢复监听
            QTimer::singleShot(300, [this](){
                cb_listener->setEnable(true); 
            });
    });

}

void MainController::restoreFocusAndPaste(){
    /*隐藏当前侧边栏，让windows自动返回焦点至上一个窗口*/
    qDebug() << ">>>【MainController】正在尝试粘贴";
    QTimer::singleShot(100, [](){
        WindowsHelper::simulate_Press_Ctrl_V();
    });
    qDebug() << ">>>【MainController】- 粘贴完成";
}