#include "FloatingBall.h"
#include <QWidget>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QDebug>
#include <QMouseEvent>
#include <windows.h>

// Qt绘图三要素
// 画家 (QPainter)：负责执行画的动作（画圆、画线、写字）。
// 画笔 (QPen)：负责轮廓（颜色、粗细、虚实）。
// 画刷 (QBrush)：负责填充（颜色、渐变、图案）。

FloatingBall::FloatingBall(QWidget *parent)
    : QWidget(parent){
    // 设置窗口格式
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool | Qt::WindowDoesNotAcceptFocus);   // 无边框、置顶、不获取焦点（但可能有焦点闪烁的问题）
    setAttribute(Qt::WA_TranslucentBackground);             // 背景透明
    setFixedSize(60, 60);                                   // 固定悬浮窗窗口大小

    // 使用windows的原生补丁，强行设置窗口属性
    #ifdef Q_OS_WIN
    HWND hwnd = (HWND)this->winId();                        // 获取本窗口的句柄
    LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
    SetWindowLong(hwnd, GWL_EXSTYLE, exStyle | WS_EX_NOACTIVATE);   // 即使用户点击了该窗口，也不尝试传递焦点给它
    #endif  // Q_OS_WIN
}

void FloatingBall::refreshUI(QSharedPointer<CopyableData> data){
    /*更新悬浮球记录的最新条目，调用QWidget.update()来唤起paintEvent*/
    currentData = data; // 更新数据引用
    update();           // 触发 paintEvent
}


void FloatingBall::paintEvent(QPaintEvent *event){
    /*每次系统唤起paintEvent事件时，都重新渲染悬浮球的显示内容*/

    // 画图：初始化一个圆
    QPainter painter(this);                                 // QPainter必须是一个局部变量，它被销毁时才会将绘图任务真正提交 
    painter.setRenderHint(QPainter::Antialiasing);          // 开启抗锯齿
    painter.setBrush(QBrush(QColor(0, 120, 215, 200)));     // 设置填充色，以笔刷形式返回
    painter.setPen(Qt::NoPen);                              // 设置画笔，且不要边框
    painter.drawEllipse(rect());                            // 以当前窗口为边界，画一个与边界相切的圆

    if(!currentData) return;                                // 排除空内容

    // 在圆圈中心显示数据内容
    switch(currentData->getType()){
        case DataType::text:{
            painter.setPen(Qt::white);                      // 显示文字时换用白色画笔
            QFont font = painter.font();
            font.setBold(true);                             // 字体加粗
            font.setPixelSize(12);                          // 设置字体大小，防止溢出
            painter.setFont(font);
            // 处理文字，显示文本的前3个字符
            QString text = currentData->getText().simplified().left(3);  
            // 居中显示文字
            painter.drawText(rect(), Qt::AlignCenter, text);    
            break;
        }

        case DataType::img:{
            QImage img = currentData->getImage();
            if(img.isNull()){
                qDebug() << ">> 【悬浮球】尝试输出图片：失败-当前数据为空";
                break;
            }

            // 保存painter的旧状态
            painter.save();

            // 创建图像显示区域的圆形裁剪路径
            QPainterPath path;
            path.addEllipse(rect().adjusted(5, 5, -5, -5)); // 向内收缩5像素
            painter.setClipPath(path);

            // 居中绘制图片，缩放图片，保持比例，平滑缩放
            QImage scaled_img = img.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
            if(scaled_img.isNull()){
                qDebug() << ">> 【悬浮球】尝试输出图片：失败-图片缩放结果为空";
                break;
            }
            // 计算缩放后的图像起始坐标
            int x = (width() - scaled_img.width()) / 2;
            int y = (height() - scaled_img.height()) / 2;
            painter.drawImage(x, y, scaled_img);

            painter.restore();                         // 恢复先前的painter设置
            break;
        }
            
        default:
            break;
    }
    
}

void FloatingBall::mousePressEvent(QMouseEvent *event){
    /*鼠标单次点击事件，是后续判断【鼠标拖拽】和【鼠标释放】的前置*/
    qDebug() << ">> 【悬浮球】发生事件[ FloatingBall::mousePressEvent ]";

    // 如果是左键单击，则需要提前准备拖拽等操作可能用到的参考点
    if(event->button() == Qt::LeftButton){      
        is_dragging = false;                        // 重置标识符
        press_start_position = event->globalPos();  // 记录点击事件的全局坐标，如果该点击事件其实是拖拽事件的开头，那么记录该起始坐标作为拖拽移动的参考点
        drag_position = event->pos();               // 记录鼠标在窗口内的相对坐标
    }
    // 如果是右键单击，则不需要记录什么数据，直接跳过判断
}

void FloatingBall::mouseMoveEvent(QMouseEvent *event){
    /*鼠标移动事件，根据移动距离来判断是否真的是拖拽*/
    
    if(event->buttons() & Qt::LeftButton){
        // 计算移动距离，如果与先前记录的鼠标单击事件中得到的“点击起始坐标”相比，移动了超过5像素，则判定为进行拖拽
        if((event->globalPos() - press_start_position).manhattanLength() > 5){
            qDebug() << ">> 【悬浮球】发生事件[ FloatingBall::mouseMoveEvent ]，且符合拖拽条件，正在进行拖拽";
            is_dragging = true; // 进入拖拽状态
            move(event->globalPos() - drag_position);   // 计算移动的方向向量
        }
    }
}

void FloatingBall::mouseReleaseEvent(QMouseEvent *event){
    /*鼠标释放事件，区分点击的是鼠标左键还是右键，并发出对应信号*/
    qDebug() << ">> 【悬浮球】发生事件[ FloatingBall::mouseReleaseEvent ]";

    if(!is_dragging){       // 如果不是正在进行拖拽，则根据左右键发出信号
        if(event->button() == Qt::LeftButton){
            qDebug() << ">> 【悬浮球】得到[ 左键点击 ]";
            emit _leftClicked();
        }
        else if(event->button() == Qt::RightButton){
            qDebug() << ">> 【悬浮球】得到[ 右键点击 ]";
            emit _rightClicked();
        }
    }
    is_dragging = false;
}