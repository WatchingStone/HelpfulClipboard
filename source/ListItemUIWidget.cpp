#include "ListItemUIWidget.h"
#include <QLabel>

ListItemUIWidget::ListItemUIWidget(QSharedPointer<CopyableData> data, QWidget *parent)
    : BaseUIWidget(data, parent) {
    // 构造时触发第一次渲染
    refreshUI();
}

void ListItemUIWidget::setLeftArea() {
    QLabel *left_area = new QLabel(this);
    
    if (data) {
        left_area->setText(data->getInfo());
    } else {
        left_area->setText("Unknown");
    }

    // 设置一些侧边栏特有的样式（可选）
    left_area->setStyleSheet("color: #666; font-size: 10px;");
    
    hlayout->addWidget(left_area);
}