#include "Sidebar.h"
#include "ListItemUIWidget.h"
#include <QListWidgetItem>
#include <QDebug>
#include <QPushButton>
#include <windows.h>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>
#include <QByteArray>


Sidebar::Sidebar(QWidget *parent)
    : QWidget(parent){
    setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::WindowDoesNotAcceptFocus);

    // 使用windows的原生补丁，强行设置窗口属性
    #ifdef Q_OS_WIN
    HWND hwnd = (HWND)this->winId();                                // 获取本窗口的句柄
    LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
    SetWindowLong(hwnd, GWL_EXSTYLE, exStyle | WS_EX_NOACTIVATE);   // 即使用户点击了该窗口，也不尝试传递焦点给它
    #endif  // Q_OS_WIN

    // 侧边栏的标题行
    headlineWidget = new QWidget(this);
    headlineLayout = new QHBoxLayout(headlineWidget);               // 侧边栏的标题行


    // 【是否常驻侧边栏】勾选框
    checkBox = new QCheckBox("常驻侧边栏", this);
    headlineLayout->addWidget(checkBox);
    headlineLayout->addStretch(1);                                  // 弹性隔离

    // 【关闭侧边栏窗口】按钮
    closeWindowBtn = new QPushButton("x", this);
    closeWindowBtn->setFixedSize(20, 20);
    headlineLayout->addWidget(closeWindowBtn);

    // 主要组件
    mainLayout = new QVBoxLayout(this);
    listWidget = new QListWidget(this);
    listWidget->setDragEnabled(false);          // 不允许listWidget自己处理拖拽
    listWidget->setAcceptDrops(false);          // 不允许接收掉落
    listWidget->setDefaultDropAction(Qt::CopyAction); 

    mainLayout->addWidget(headlineWidget, 0);

    // 管理listWidget布局-提前配置一个缩放策略
    QSizePolicy mlayout_policy;
    mlayout_policy.setHorizontalPolicy(QSizePolicy::Fixed);
    mlayout_policy.setVerticalPolicy(QSizePolicy::Expanding);
    // 管理布局-设置窗口主布局
    mainLayout->addWidget(listWidget, 10);      // 在主布局中添加QListWidget，设置拉伸权重为10
    listWidget->setSizePolicy(mlayout_policy);
    listWidget->setMinimumWidth(100);           // 设置最小宽度

    // 在侧边栏中添加一个【退出程序】的按钮
    exit_btn = new QPushButton("退出程序", this);
    exit_btn->setStyleSheet("background-color: #66ccff; color: white; border-radius: 5px; padding: 5px;");
    mainLayout->addWidget(exit_btn, 1);

    // 连接信号槽
    // 当侧边栏内的列表元素发出“点击”信号时，读取返回的对应“被点击元素”的指针，
    connect(listWidget, &QListWidget::itemClicked, this, [this](QListWidgetItem *item){
        // 终端输出回显，检查点击条目属性
        QVariant v = item->data(Qt::UserRole);
        qDebug() << ">>> QVariant type:" << v.typeName();   // 调试代码：查看 QVariant 里的类型名称

        if(item->data(ItemDataAttribute::Datatype) != ItemDataAttribute::imgData){
            qDebug() << ">>> 鼠标单击：" << item->text() << item->data(ItemDataAttribute::textData).toString();
        }
        else{
            qDebug() << ">>> 鼠标单击【图片img条目】：" << item->text();
        }

        // 对点击条目的具体处理，处理回写剪贴板
        // 将数据以智能指针的形式读取出来
        if(v.canConvert<QSharedPointer<CopyableData>>()){
            QSharedPointer<CopyableData> d = v.value<QSharedPointer<CopyableData>>();   
            qDebug() << ">>> 发送信号[_itemSeleceted]，读取条目的智能指针：" << d->getText();
            emit _itemSelected(d);
        }
    });

    /*绑定【常驻保持侧边栏】按钮*/
    connect(checkBox, &QCheckBox::toggled, this, [this](bool checked){
        // 当侧边栏内的按钮组件被按下时，发出信号，等待MainContorller接收
        emit _keepSidebarBtnPressed(checked);
    });

    /*绑定【关闭侧边栏窗口】按钮*/
    connect(closeWindowBtn, &QPushButton::clicked, this, [this](){
        this->hide();
    });
    
    /*绑定【退出程序】按钮*/
    connect(exit_btn, &QPushButton::clicked, qApp, &QApplication::quit);


    // 要先初始化所有变量，后绑定逻辑
    headlineWidget->installEventFilter(this);           // 给子组件设置上层过滤器，由父组件提前拦截并分析事件
    listWidget->installEventFilter(this);               // 将本Sidebar设为过滤器，用来拦截处理对于列表元素的拖拽事件
    listWidget->viewport()->installEventFilter(this);


    setMinimumWidth(200);
    resize(200, 500);
}

void Sidebar::updateList(const QList<QSharedPointer<CopyableData>> &history){
    /*接收来自ClipboardManager剪贴板管理器的当前数据历史条目列表，渲染出来*/
    listWidget->clear();
    for(const QSharedPointer<CopyableData> d : history){
        // 将读取到的每条数据都重新加载进listWidget中
        QListWidgetItem *item = new QListWidgetItem();
        QVariant v = QVariant::fromValue(d);    // 显式使用.fromValue()将智能指针转换为QVariant，这样才能准确放入QList中
        item->setData(Qt::UserRole, v);         // 将原始数据封装进item中
        listWidget->addItem(item);              // 将item作为QList的一个元素

        // 使用自定义的UI显示组件样式替换list中的默认样式
        ListItemUIWidget *customItemUIWidget = new ListItemUIWidget(d);
        listWidget->setItemWidget(item, customItemUIWidget);                // 强制list使用自定义的组件显示该行
        item->setSizeHint(customItemUIWidget->sizeHint());                  // 调整占位符的大小
    }
}

bool Sidebar::eventFilter(QObject *obj, QEvent *event){
    /*对于下属子组件设置的过滤器，指向子组件的事件会首先经过本父组件进行处理*/

    // qDebug() << ">> 【Sidebar】正在过滤事件";

    // 如果事件来自于标题行（含勾选框的空白处）—— 尝试处理“通过拖拽侧边栏的可用区域实现移动侧边栏位置”
    if(obj == headlineWidget || obj == checkBox){
        // qDebug() << ">> 【Sidebar】正在过滤事件 - 事件来源：headlineWidget or checkBox";

        // 判断具体是否是鼠标的相关事件
        if(event->type() == QEvent::MouseButtonPress){      // 如果是鼠标点击事件，则正常传递给子组件
            // qDebug() << ">> 【Sidebar】正在过滤事件 - 鼠标点击 press 事件";

            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);     // 父类转子类，向下转换不安全，所以需要手动用static_cast转换，因为前面已经判断过具体的类型确实是QMouseEvent 
            if(mouseEvent->button() == Qt::LeftButton){     // 对于左键单击事件，提前记录可能用到的移动坐标
                press_start_position = mouseEvent->globalPos();
                // drag_position = mouseEvent->globalPos() - this->frameGeometry().topLeft();
                drag_position = mouseEvent->globalPos() - this->pos();

                qDebug() << ">> 【Sidebar】正在过滤【headlineWidget】事件 - press 事件 - 左键点击 - drag_position : " << drag_position;

                return false;                               // 返回false可以让事件继续传给子组件
            }
        }
        else if(event->type() == QEvent::MouseMove){        // 如果是鼠标移动事件，则过滤，由本组件提前处理，执行拖拽逻辑
            // qDebug() << ">> 【Sidebar】正在过滤事件 - 鼠标移动 move 事件";

            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if(mouseEvent->buttons() & Qt::LeftButton){
                // qDebug() << ">> 【Sidebar】正在过滤事件 - move 事件 - 左键拖动";

                if((mouseEvent->globalPos() - press_start_position).manhattanLength() > 5){
                    qDebug() << ">> 【Sidebar】正在过滤【headlineWidget】事件 - move 事件 - 左键拖动 - drag_position : " << drag_position;
                    move(mouseEvent->globalPos() - drag_position);
                    return true;                                // 返回true，拦截事件，不发送给子组件
                }          
            }
        }
    }

    // 处理“对于listWidget元素的拖拽事件”
    // 由于拖拽事件不是由listWidget本身处理，而是由listWidget->viewport()处理
    // 所以不是检查(obj == listWidget)而是检查(obj == listWidget->viewport())
    if (obj == listWidget->viewport()) {   
        
        if (event->type() == QEvent::MouseButtonPress) {
             qDebug() << ">> 【Sidebar】正在过滤【listWidget】事件 - press 事件";

            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton) {
                // 记录鼠标按下的初始位置（相对于列表）
                this->press_start_position = mouseEvent->pos(); 
            }
            return false; 
        }
       
        else if(event->type() == QEvent::MouseMove) {
            // qDebug() << ">> 【Sidebar】正在过滤【数据列表】事件 - move 事件";

            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if(mouseEvent->buttons() & Qt::LeftButton){


                // 检查移动距离是否达到了启动拖拽的标准 (通常是 5 到 10 像素)
                if ((mouseEvent->pos() - press_start_position).manhattanLength() < QApplication::startDragDistance()) {
                    return false;
                }

                // QListWidgetItem *item = listWidget->itemAt(mouseEvent->pos()); // 获取鼠标位置下的条目
                QListWidgetItem *item = listWidget->currentItem();
                if(item){   // 如果目标是listWidget，如果事件是鼠标移动事件，如果含有左键点击，如果listWidget当前选中元素有效
                    qDebug() << ">> 【Sidebar】正在过滤【listWidget】事件 - move 事件";

                    // 创建拖拽对象
                    QDrag *drag = new QDrag(this);          // 用来执行拖拽功能
                    QMimeData *mimeData = new QMimeData;    // 拖拽过程需要保存中间数据

                    // 取出listWidget内的item数据（即原始的数据指针 QSharedPointer<CopyableData>）
                    QVariant v = item->data(Qt::UserRole);
                    if(v.canConvert<QSharedPointer<CopyableData>>()){
                        QSharedPointer<CopyableData> data_ptr = v.value<QSharedPointer<CopyableData>>();

                        // 将数据指针转为字节数组，存入mimeData。这是程序内部传输最快的方式
                        // // 先从智能指针中用.data()取出数据原地址
                        // // 再进行显式类型转换，转为quintptr类型的指针
                        // // 将新指针按照数字形式翻译为字节数组，用于底层传递
                        // // QByteArray data_address = QByteArray::number((quintptr)data_ptr.data());   

                        QByteArray data_address;                                    // 初始化一个目标地址缓存
                        QDataStream data_stream(&data_address, QIODevice::WriteOnly);   // 构造数据流对象，以data_address为缓存
                        data_stream << *data_ptr;                                   // 利用 DataStruct 重载的流处理和序列化方法

                        mimeData->setData(MIME_TYPE_COPYABLE_DATA, data_address);   // 存入mimeData
                        drag->setMimeData(mimeData);                                // 存入drag
                        // drag->setPixmap(...) // 设置拖拽时的图标
                        drag->exec(Qt::CopyAction);                                 // 开始执行拖拽
                        return true;    // 拦截事件，不让拖拽事件发送给listWidget
                    }
                }
            }
        }
    }



    return QWidget::eventFilter(obj, event);                // 如果上面2个需要特殊处理的逻辑不满足，则按默认机制运行
}


// void Sidebar::mousePressEvent(QMouseEvent *event){
//     /*拖拽的起点*/ 
//     if(event->button() == Qt::LeftButton){
//         qDebug() << ">> 【侧边栏】被点击";
//         press_start_position = event->globalPos();
//         drag_position = event->pos();
//     }
// }

// void Sidebar::mouseMoveEvent(QMouseEvent *event){
//     /*判断位移距离*/ 
//     if(event->buttons() & Qt::LeftButton){
//         if((event->globalPos() - press_start_position).manhattanLength() > 5){
//             qDebug() << ">> 【侧边栏】正在被拖拽";
//             move(event->globalPos() - drag_position);
//         }
//     }
// }

// void Sidebar::mouseReleaseEvent(QMouseEvent *event){
//     /*结束一次拖拽*/
// }