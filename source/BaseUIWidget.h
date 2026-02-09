// BaseUIWidget 是 ListItemUIWidget 和 SlotUIWidget 的基类
// 提供初始化与渲染数据的方法
#ifndef BASEUIWIDGET_H
#define BASEUIWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QSharedPointer>
#include "DataStruct.h"

class BaseUIWidget : public QWidget{
    Q_OBJECT
    protected:
        QHBoxLayout *hlayout;               // 横向控制左右两个区域的组件
        QSharedPointer<CopyableData> data;  // 数据指针，子类不必再单独存储数据

        // 在基类中定义通用的渲染函数
        QPixmap createThumbnail(const QImage& img, int w, int h);           // 对于图像：生成缩略图
        QString createTruncatedText(const QString& text, int len_limit);    // 对于文本：生成压缩文本
    
    private:
        void clearLayout();                                 // 清空组件

    public:
        explicit BaseUIWidget(
            QSharedPointer<CopyableData> data = nullptr,    // 构造函数不一定需要用到data
            QWidget *parent = nullptr
        );
        virtual ~BaseUIWidget() = default;

        void refreshUI();
        void setData(QSharedPointer<CopyableData> data);    // 设置数据
        virtual void setLeftArea() = 0;
        virtual void setRightArea();
};

#endif // BASEUIWIDGET_H