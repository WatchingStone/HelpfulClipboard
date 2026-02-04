// 存储数据的核心组件，不关心数据的显示
#ifndef CLIPBOARDMANAGER_H
#define CLIPBOARDMANAGER_H

#include <QObject>
#include <QList>
#include <QSharedPointer>
#include "DataStruct.h"

class ClipboardManager : public QObject{
    Q_OBJECT

    private:
        QList<QSharedPointer<CopyableData>> history;                        // 存储数据的核心，以QList<QSharedPointer<CopyableData>>形式保存
        int HISTORY_SIZE_LIMIT;                                             // 历史条目的最大记录数，超出的条目将被删除

    public:
        explicit ClipboardManager(int list_size_limit = 5, QObject *parent = nullptr);

        QSharedPointer<CopyableData> getLatest() const;                     // 获得当前记录的最新数据条目
        const QList<QSharedPointer<CopyableData>>& getHistory() const;      // 获取当前的完整列表

    signals:
        void _historyChanged(); // 当数据发生增、删、改时发出信号，通知UI层刷新显示
        void _lastestDataChanged(QSharedPointer<CopyableData> data);        // 记录的最新一条数据发生更改的信号，专用于通知悬浮球

    public slots:
        void _on_get_new_data_DO_add_data(QSharedPointer<CopyableData> data);   // 当从监听器等地方获得新数据时，插入到记录历史中
        void _on_needed_DO_clear_history();                                     // 如果有必要，则清除历史记录
    };

#endif // CLIPBOARDMANAGER_H