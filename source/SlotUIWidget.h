// SlotUIWidget专用于快捷栏槽位的数据显示，继承自BaseUIWidget

#ifndef SLOTUIWIDGET_H
#define SLOTUIWIDGET_H

#include <QWidget>
#include "BaseUIWidget.h"

class SlotUIWidget : public BaseUIWidget{
    private:
        int id;
    public:
        explicit SlotUIWidget(int id, QSharedPointer<CopyableData> data = nullptr, QWidget *parent = nullptr);
        void setLeftArea() override;
        void setRightArea() override;
};

#endif // SLOTUIWIDGET_H