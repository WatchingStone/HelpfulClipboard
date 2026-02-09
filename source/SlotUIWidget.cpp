#include "SlotUIWidget.h"
#include <QWidget>
#include <QLabel>
#include <QString>

SlotUIWidget::SlotUIWidget(int id, QSharedPointer<CopyableData> data, QWidget *parent)
    : id(id), BaseUIWidget(data, parent){
    refreshUI();
}

void SlotUIWidget::setLeftArea(){
    QLabel *left_area = new QLabel(QString::number(id + 1), this);
    left_area->setStyleSheet("font-weight: bold; color: #0078d7;");
    hlayout->addWidget(left_area);
}

void SlotUIWidget::setRightArea(){
    /*重写右侧显示区域*/
    QLabel *right_area = new QLabel(this);
    right_area->setFixedSize(50, 50);
    
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