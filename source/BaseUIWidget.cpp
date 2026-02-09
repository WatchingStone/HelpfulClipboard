
#include "BaseUIWidget.h"
#include <QPixmap>
#include <QImage>
#include <QString>
#include <QLabel>
#include <QLayoutItem>

BaseUIWidget::BaseUIWidget(QSharedPointer<CopyableData> data, QWidget *parent)
    : data(data), QWidget(parent){
    // 让整个自定义组件及其子控件不接收鼠标事件，这样点击就会“穿透”到下层的 ListWidgetItem
    this->setAttribute(Qt::WA_TransparentForMouseEvents);
    hlayout = new QHBoxLayout(this);
    hlayout->setContentsMargins(5, 5, 5, 5);
}

void BaseUIWidget::setData(QSharedPointer<CopyableData> d){
    /*更新data，同时刷新显示*/
    data = d;
    refreshUI();
}

QPixmap BaseUIWidget::createThumbnail(const QImage& img, int w, int h){
    /*对于图像：生成缩略图*/
    return QPixmap::fromImage(img.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation));
} 

QString BaseUIWidget::createTruncatedText(const QString& text, int len_limit){
    /*对于文本：生成压缩文本*/
    QString t = text.simplified();
    // 对于长度超过限制的文字进行压缩
    if(t.length() > len_limit){
        t = t.left(len_limit >> 2) + "..." + t.right(len_limit >> 2);    // 取首尾各一部分字符，中间用“...”省略
    }
    return t;
}

void BaseUIWidget::refreshUI(){
    /*刷新显示*/
    clearLayout();
    setLeftArea();
    setRightArea();
}

void BaseUIWidget::clearLayout(){
    /*清空组件*/
    QLayoutItem *child;
    while((child = hlayout->takeAt(0)) != nullptr){ // 尝试从hlayout中读出剩下第一个元素，如果读取成功，说明还没清空
        if(child->widget()) child->widget()->deleteLater(); // 安全删除旧组件
        delete child;
    }
}

void BaseUIWidget::setRightArea(){
    /*显示右侧区域*/ 
    QLabel *right_area = new QLabel(this);
    
    if (!data) {
        // --- 处理空状态 ---
        right_area->setText("-");
        right_area->setStyleSheet("color: gray; font-style: italic;");
    } else {
        // --- 显示数据 ---
        if (data->getType() == DataType::img) {
            right_area->setPixmap(createThumbnail(data->getImage(), 50, 50));
        } else {
            right_area->setText(createTruncatedText(data->getText(), 30));
        }
    }
    hlayout->addWidget(right_area);
}


