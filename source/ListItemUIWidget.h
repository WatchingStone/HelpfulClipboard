// ListItemUIWidget.h
// 侧边栏列表项的显示组件，继承自 BaseUIWidget，用来替换QListWidget的默认显示方式，使其能够自定义显示文本和图片缩略图

#ifndef LISTITEMUIWIDGET_H
#define LISTITEMUIWIDGET_H

#include "BaseUIWidget.h"

class ListItemUIWidget : public BaseUIWidget {
    Q_OBJECT

public:
    // 侧边栏通常在创建时就一定有数据，所以这里不设默认空指针
    explicit ListItemUIWidget(QSharedPointer<CopyableData> data, QWidget *parent = nullptr);
    
    // 实现基类的纯虚函数：左侧显示数据的 Info 描述
    void setLeftArea() override;
};

#endif // LISTITEMUIWIDGET_H