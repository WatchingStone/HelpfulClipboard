// ListItemUIWidget.cpp
// 自定义的数据显示组件，用来替换QListWidget的默认显示方式，使其能够自定义显示文本和图片缩略图
#include "ListItemUIWidget.h"
#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include "DataStruct.h"

ListItemUIWidget::ListItemUIWidget(QSharedPointer<CopyableData> data, QWidget *parent)
    : QWidget(parent), IconSize_width(50), IconSize_height(50), text_len_limit(30){
        // 让整个自定义组件及其子控件不接收鼠标事件，这样点击就会“穿透”到下层的 ListWidgetItem
        this->setAttribute(Qt::WA_TransparentForMouseEvents);

        // 主布局
        QHBoxLayout *layout = new QHBoxLayout(this);
        layout->setContentsMargins(5, 5, 5, 5);

        // 一个框主要分为2部分，左部分可能用来标号，右部分用来显示图片或文字
        QLabel *left_area = new QLabel(this);   // 左侧留作备用
        QLabel *right_area = new QLabel(this);  // 右侧显示剪贴板内容，图片和文字

        left_area->setText(data->getInfo());    // 左侧显示特殊信息
        
        // 根据传入的剪贴板内容的具体类型来决定如何显示
        if(data->getType() == DataType::img){   // @@@ 对于图片，生成缩略图
            // 先配置图片显示格式
            right_area->setFixedSize(IconSize_width, IconSize_height);              // 固定大小的缩略图尺寸
            right_area->setStyleSheet("border: 1px solid #ccc; background: #eee;"); // 设置风格
            right_area->setAlignment(Qt::AlignCenter);                              // 居中对齐

            // 制作缩略图
            QImage thumbnail = data->getImage().scaled(IconSize_width, IconSize_height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            right_area->setPixmap(QPixmap::fromImage(thumbnail));                   // QImage的像素处理更快，QPixmap的显示速度更快
            right_area->setToolTip(QString::number(data->getImage().width()) + "x" + QString::number(data->getImage().height()));   // 图片尺寸信息
        }
        else{                                   // @@@ 对于文字，生成省略的文字（目前对于text、url等数据都将data.type设置为text）
            QString rawText = data->getText();                  // 加载原文本
            QString pureText = rawText.simplified();            // 去除换行符
            // 对于长度超过限制的文字进行压缩
            if(pureText.length() > text_len_limit){
                pureText = pureText.left(text_len_limit >> 2) + "..." + pureText.right(text_len_limit >> 2);    // 取首尾各一部分字符，中间用“...”省略
            }
            right_area->setText(pureText);
            right_area->setWordWrap(true);      // 自动换行
        }

        layout->addWidget(left_area);
        layout->addWidget(right_area);
    }