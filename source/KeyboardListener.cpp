#include "KeyboardListener.h"
#include <windows.h>
#include <QDebug>

const int HOTKEY_ID_BASE = 6666;    // 定义热键起始ID，避免与其他热键冲突

KeyboardListener::KeyboardListener(QWidget *parent)
    : QWidget(parent){
    this->hide();           // 首先将本窗口隐藏，因为本监听器只是一个辅助窗口
    registerHotkeys();      // 初始化时注册热键
}

KeyboardListener::~KeyboardListener(){
    unregisterHotkeys();    // 析构时取消注册
}

void KeyboardListener::registerHotkeys(){
    /*注册热键，默认注册【ctrl + 数字键1~9】*/
    qDebug() << ">> 【KeyboardListener】正在注册热键";

    HWND hwnd = (HWND)this->winId();    // 获取当前窗口的句柄
    for(int i = 0; i < 9; ++i){
        if(RegisterHotKey(hwnd, HOTKEY_ID_BASE + i, MOD_CONTROL, 0x31 + i)){
            // 注册热键，传入窗口句柄hwnd，设置新热键id，使用ctrl，绑定数字键（0x31对应数字“1”键）
            qDebug() << ">> 【KeyboardListener】成功注册热键：ctrl + " << (i + 1);
        }
        else{
            qDebug() << ">> 【KeyboardListener】注册失败：ctrl + " << (i + 1);
        }
    }
}

void KeyboardListener::unregisterHotkeys(){
    /*由析构函数调用，删除已注册的热键*/
    qDebug() << ">> 【KeyboardListener】正在清除热键";

    HWND hwnd = (HWND)this->winId();
    for(int i = 0; i < 9; ++i){
        UnregisterHotKey(hwnd, HOTKEY_ID_BASE + i);
    }
}

bool KeyboardListener::nativeEvent(const QByteArray &eventType, void *message, long *result){
    /*捕获本地事件，提取出已设置的快捷键并发送信号*/
    MSG *msg = static_cast<MSG*>(message);      // 将指针转为windows消息指针
    if(msg->message == WM_HOTKEY){              // 如果该消息是热键事件，则处理
        // 提取出该热键事件的消息
        int hotkeyId = msg->wParam;             // 全局热键消息id
        int slotId = hotkeyId - HOTKEY_ID_BASE; // 还原为已设置的快捷键id
        if(slotId >= 0 && slotId <= 8){
            qDebug() << ">> 【KeyboardListener】监听到热键事件：slotId = " << slotId + 1;
            emit _slotHotkeyPressed(slotId);
            return true;    // 表示该消息已被处理，不必继续下发
        }
    }
    return QWidget::nativeEvent(eventType, message, result);
}