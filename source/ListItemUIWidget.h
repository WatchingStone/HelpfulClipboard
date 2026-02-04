// ListItemUIWidget.h
// 自定义的数据显示组件，用来替换QListWidget的默认显示方式，使其能够自定义显示文本和图片缩略图

#ifndef LISTITEMUIWIDGET_H
#define LISTITEMUIWIDGET_H 

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include "DataStruct.h"

class ListItemUIWidget : public QWidget{
    Q_OBJECT

    private:
        int IconSize_width, IconSize_height;    // 缩略图大小
        int text_len_limit;                     // 文本最长输出大小，超过的就要进行省略输出
    
    public:
        explicit ListItemUIWidget(QSharedPointer<CopyableData> data, QWidget *parent = nullptr);
};

#endif  // LISTITEMUIWIDGET_H