cmake_minimum_required(VERSION 3.5)

# 项目名称
project(HelpfulClipBoard LANGUAGES CXX)

# --- 1. 设置 C++ 标准 (Qt 5 通常配合 C++11 或 C++14) ---
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- 2. 开启 Qt 自动化 (Qt 5 必须手动写这些，Qt 6 那个报错的命令就是封装了这三行) ---
set(CMAKE_AUTOMOC ON) # 自动处理元对象系统 (信号与槽)
set(CMAKE_AUTORCC ON) # 自动处理资源文件 (.qrc)
set(CMAKE_AUTOUIC ON) # 自动处理 UI 文件 (.ui)

# --- 3. 寻找 Qt 5 库 ---
# 这里的 REQUIRED 意味着找不到报错
# COMPONENTS 后面跟你需要用到的模块，比如 Widgets, Network, Sql 等
find_package(Qt5 COMPONENTS Widgets REQUIRED)

# --- 4. 添加源文件 ---
# 如果你有 main.cpp, mainwindow.cpp 等，都在这里列出
# 也可以用 file(GLOB_RECURSE SRCS *.cpp *.h) 自动扫描，但手动指定更规范
add_executable(HelpfulClipBoard
    source/SidebarWidget.cpp
    source/SidebarWidget.h
    source/MainController.cpp
    source/MainController.h
    source/FloatingBall.cpp
    source/FloatingBall.h
    source/ClipboardManager.cpp
    source/ClipboardManager.h
    source/WindowsHelper.cpp
    source/WindowsHelper.h
    source/ListItemUIWidget.cpp
    source/ListItemUIWidget.h
    source/ClipboardListener.cpp 
    source/ClipboardListener.h 
    source/DataStruct.cpp 
    source/DataStruct.h 
    source/main.cpp 
)

# --- 5. 链接 Qt 库 ---
# 注意这里是 Qt5::Widgets，而不是 Qt6
target_link_libraries(HelpfulClipBoard PRIVATE Qt5::Widgets)