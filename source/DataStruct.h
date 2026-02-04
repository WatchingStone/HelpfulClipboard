// 剪贴板保存的数据，使用单独的数据结构进行存储，可以用在QListWidget的QListWidgetItem里或其他地方
#ifndef DATASTRUCT_H
#define DATASTRUCT_H

#include <QApplication>
#include <QVariant>
#include <QImage>
#include <QString>
#include <QUrl>
#include <QObject>
#include <QList>
#include <QMetaType>      // 包含元对象系统基础头文件，用来声明（注册）自定义数据类型的智能指针
#include <QSharedPointer> // 同上


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
};


// 必须在头文件中，且在任何命名空间之外
// Qt 的元对象系统需要预先知道你的自定义类型，才能将其“包装”进 QVariant
// 虽然在构造函数里写了 qRegisterMetaType（这是运行时注册），但编译器在处理 v.value<T>() 时需要编译时的声明
Q_DECLARE_METATYPE(QSharedPointer<CopyableData>)    

#endif // DATASTRUCT_H