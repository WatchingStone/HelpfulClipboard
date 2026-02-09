// KeyboardListener 键盘监听器，用来监听快捷键的触发，发送信号

#ifndef KEYBOARDLISTENER_H
#define KEYBOARDLISTENER_H

#include <QWidget>
#include <QSharedPointer>

class KeyboardListener : public QWidget{
    Q_OBJECT
    public:
        explicit KeyboardListener(QWidget *parent = nullptr);
        ~KeyboardListener();

    signals:
        void _slotHotkeyPressed(int slotId);    // 快捷栏槽位的快捷键被触发，发射槽位id信号
    
    protected:
        // 重写原生的事件过滤器，捕获windows发送给本窗口的快捷键信息
        bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;
    
    private:
        void registerHotkeys();                 // 注册热键
        void unregisterHotkeys();               // 取消注册
};


#endif // KEYBOARDLISTENER_H