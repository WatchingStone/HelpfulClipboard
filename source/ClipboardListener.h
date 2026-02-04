#ifndef CLIPBOARDLISTENER_H
#define CLIPBOARDLISTENER_H
#include "DataStruct.h"         // 包含 CopyableData 和 DataType 的定义
#include <QMimeData>
#include <QClipboard>
#include <QString>
#include <QImage>
#include <QUrl>
#include <QDebug>
#include <QApplication>         // 用于获取 QApplication::clipboard()
#include <QVariant>             // 用于 qvariant_cast
#include <QSharedPointer>       // Qt的智能指针
#include <QObject>
#include <QSharedPointer> // Qt的智能指针

class ClipboardListener : public QObject
{
    Q_OBJECT // 元对象系统宏

public:
    // 构造函数
    explicit ClipboardListener(QObject *parent = nullptr);
    void setEnable(bool enable);    // 是否启用剪贴板监听器。如果是本程序主动修改的剪贴板，则无需处理剪贴板变更事件

signals:
    // 信号声明：当从剪贴板获取数据时发出
    void _getClipboardData(QSharedPointer<CopyableData> W);

private slots:
    // 私有槽函数：响应系统剪贴板变化
    void _on_clipboard_data_changed();

    // 私有成员函数：执行具体的转换逻辑
    void Clipboard2Data();

private:
    QClipboard *clipboard;  // 剪贴板指针
    bool is_enabled;        // 是否启用剪贴板监听器。如果是本程序主动修改的剪贴板，则无需处理剪贴板变更事件
};

#endif // CLIPBOARDLISTENER_H
