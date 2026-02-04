//  专门监听剪贴板变化的类，将剪贴板内容转化成自定义的DataStruct
#include "ClipboardListener.h"
#include "DataStruct.h"         // 包含 CopyableData 和 DataType 的定义
#include <QMimeData>
#include <QClipboard>
#include <QString>
#include <QImage>
#include <QUrl>
#include <QDebug>
#include <QApplication>         // 用于获取 QApplication::clipboard()
#include <QVariant>             // 用于 qvariant_cast
#include <QObject>
#include <QSharedPointer>       // Qt的智能指针



void ClipboardListener::setEnable(bool enable){
    is_enabled = enable;
}
void ClipboardListener::_on_clipboard_data_changed(){
    if(!this->is_enabled) return;           // 是否启用剪贴板监听器。如果是本程序主动修改的剪贴板，则无需处理剪贴板变更事件
    Clipboard2Data();
}

void ClipboardListener::Clipboard2Data(){
    /*将剪贴板内容根据数据类型转换为特定处理类*/

    const QMimeData *mime_data = clipboard->mimeData(); // 读取剪贴板
    QSharedPointer<CopyableData> ndata(new CopyableData()); // 创建一个（智能指针）数据对象用来存储从剪贴板读取的数据，无论读到的内容合不合法，都会用信号发射出去

    if(mime_data->hasImage()){
        QImage img = qvariant_cast<QImage>(mime_data->imageData());
        qDebug() << ">>> 剪贴板当前包含Img";
        ndata->setType(DataType::img);       // 更新信号对象
        ndata->setImg(img);
        ndata->setInfo("img");
    }
    else if(mime_data->hasText()){
        QString text = QString(mime_data->text());
        qDebug() << ">>> 剪贴板当前包含text";
        ndata->setType(DataType::text);
        ndata->setText(text);
        ndata->setInfo("text");
    }
    else if(mime_data->hasUrls()){
        QList<QUrl> urls = QList<QUrl>(mime_data->urls());
        QString urltext = "";               // 对于 urls 数据，需要额外保存一份text形式的数据

        for(QUrl url : urls){
            qDebug() << ">>> 剪贴板当前包含URL，文件路径：" << url.toLocalFile();
            urltext += url.toLocalFile() + " \n";
        }
        ndata->setType(DataType::urls);
        ndata->setUrls(urls);
        ndata->setText(urltext);
        ndata->setInfo("urls");
    }

    // 即使剪贴板内容不是以上任何一种类型，也可以按照默认的others类型发送信号
    emit _getClipboardData(ndata);
}


ClipboardListener::ClipboardListener(QObject *parent) : QObject(parent), is_enabled(true){
    /*在构造函数中连接信号*/
    clipboard = QApplication::clipboard();  // 获取全局唯一的剪贴板对象
    connect(clipboard, &QClipboard::dataChanged, this, &ClipboardListener::_on_clipboard_data_changed);
}


