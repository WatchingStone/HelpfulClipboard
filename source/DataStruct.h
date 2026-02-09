// 剪贴板保存的数据，使用单独的数据结构进行存储，可以用在QListWidget的QListWidgetItem里或其他地方
#ifndef DATASTRUCT_H
#define DATASTRUCT_H

// 定义一个唯一的 Mime 类型字符串，用于从【侧边栏】拖拽到【快捷栏】过程中的数据传递
#define MIME_TYPE_COPYABLE_DATA "application/x-copyable-data-ptr"

#include <QApplication>
#include <QVariant>
#include <QImage>
#include <QString>
#include <QUrl>
#include <QObject>
#include <QList>
#include <QMetaType>        // 包含元对象系统基础头文件，用来声明（注册）自定义数据类型的智能指针
#include <QSharedPointer>   // 同上
#include <QDataStream>      //增加对 QDataStream 的支持。这样可以直接把对象写入文件或拖拽流


/*数据类型*/
enum DataType{
    text,
    img,
    urls,
    others,
};

class CopyableData{
    /*可复制粘贴的数据对象*/
    private:
        DataType type;
        QString text;
        QImage img;             // QImage的像素处理更快，QPixmap的显示速度更快
        QList<QUrl> urls;
        QString info;           // 其他描述信息

    public:
        CopyableData(DataType t=DataType::others); // 设置数据类型
        // const CopyableData& getData();      // 获取数据

        void setType(DataType t);
        void setText(QString &s);
        void setImg(QImage &i);
        void setUrls(QList<QUrl> &q);
        void setInfo(QString s);

        DataType getType();
        QString getText();
        QImage getImage();
        QList<QUrl> getUrls();
        QString getInfo();

        // 添加友元操作符，支持数据流的序列化，用于传递
        friend QDataStream &operator<<(QDataStream &out, const CopyableData &data);
        friend QDataStream &operator>>(QDataStream &in, CopyableData &data);
};

// 数据流操作符声明
QDataStream &operator<<(QDataStream &out, const CopyableData &data);
QDataStream &operator>>(QDataStream &in, CopyableData &data);



// 必须在头文件中，且在任何命名空间之外
// Qt 的元对象系统需要预先知道你的自定义类型，才能将其“包装”进 QVariant
// 虽然在构造函数里写了 qRegisterMetaType（这是运行时注册），但编译器在处理 v.value<T>() 时需要编译时的声明
Q_DECLARE_METATYPE(QSharedPointer<CopyableData>)    

#endif // DATASTRUCT_H