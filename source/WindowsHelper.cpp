#define NOMINMAX
#include <windows.h>
#include "WindowsHelper.h"
#include <QDebug>

namespace WindowsHelper{
    void simulate_Press_Ctrl_V(){
        /*模拟键盘按键Ctrl+V事件*/

        // 关键点：如果你的程序窗口不接受焦点，那么 ForegroundWindow 应该是你之前的编辑器
        // 我们不需要 SetForegroundWindow，因为我们根本没抢走它
        // 但我们可以强行检查当前谁是前台
        HWND hwnd = GetForegroundWindow();
        if (hwnd == NULL) {
            qDebug() << ">> 【WindoesHelper】：错误！没有上一个窗口";
        }

        
        INPUT inputs[4];    // 分为【按下ctrl】、【按下v】、【松开v】、【松开ctrl】，共4个事件
        ZeroMemory(inputs, sizeof(inputs));     // 初始化内存

        // 设置4个按键事件
        inputs[0].type = INPUT_KEYBOARD;        // 输入事件属性为键盘按键
        inputs[0].ki.wVk = VK_CONTROL;          // 按下ctrl

        inputs[1].type = INPUT_KEYBOARD;
        inputs[1].ki.wVk = 'V';                 // 按下v

        inputs[2].type = INPUT_KEYBOARD;
        inputs[2].ki.wVk = 'V';
        inputs[2].ki.dwFlags = KEYEVENTF_KEYUP; // 松开v

        inputs[3].type = INPUT_KEYBOARD;
        inputs[3].ki.wVk = VK_CONTROL;
        inputs[3].ki.dwFlags = KEYEVENTF_KEYUP; // 松开ctrl

        SendInput(4, inputs, sizeof(INPUT));    // 发送事件
    }
}