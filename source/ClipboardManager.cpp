#include "ClipboardManager.h"

ClipboardManager::ClipboardManager(int list_size_limit, QObject *parent)
    : QObject(parent), HISTORY_SIZE_LIMIT(list_size_limit){
        /*构造函数*/
}

QSharedPointer<CopyableData> ClipboardManager::getLatest() const {
    /*获得当前历史记录中的最新数据条目，需要注意判空！！！*/
    if(history.empty())     return nullptr;
    return history.front();
}

const QList<QSharedPointer<CopyableData>>& ClipboardManager::getHistory() const {
    /*获取当前的完整历史记录*/
    return history;
}

void ClipboardManager::_on_get_new_data_DO_add_data(QSharedPointer<CopyableData> data){
    /*当从剪贴板监听器或其他组件获得新数据时，添加进历史记录，删除超出数量上限的末尾条目*/
    history.prepend(data);                                      // 在列表开头插入新元素
    if(history.size() > HISTORY_SIZE_LIMIT) history.pop_back(); // 借助智能指针的特性自动释放资源
    emit _historyChanged();
    emit _lastestDataChanged(data);
}

void ClipboardManager::_on_needed_DO_clear_history(){
    /*当有必要时，清空现有历史记录*/
    history.clear();
    emit _historyChanged();
    emit _lastestDataChanged(nullptr);
}