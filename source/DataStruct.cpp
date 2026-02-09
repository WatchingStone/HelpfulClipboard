// 剪贴板保存的数据，使用单独的数据结构进行存储，可以用在QListWidget的QListWidgetItem里或其他地方
#include "DataStruct.h"
#include <QApplication>
#include <QVariant>
#include <QImage>
#include <QString>
#include <QUrl>
#include <QObject>
#include <QList>


CopyableData::CopyableData(DataType t): type(t), info("unknown"){} // 设置数据类型

// const CopyableData& CopyableData::getData(){}     // 获取数据

void CopyableData::setType(DataType t){
    type = t;
}

void CopyableData::setText(QString &s){
    text = s;
}

void CopyableData::setImg(QImage &i){
    img = i;
}

void CopyableData::setUrls(QList<QUrl> &q){
    urls = q;
}

void CopyableData::setInfo(QString s){
    info = s;
}

DataType CopyableData::getType(){
    return type;
}
QString CopyableData::getText(){
    return text;
}
QImage CopyableData::getImage(){
    return img;
}
QList<QUrl> CopyableData::getUrls(){
    return urls;
}
QString CopyableData::getInfo(){
    return info;
}

QDataStream &operator<<(QDataStream &out, const CopyableData &data){
    /*流操作符重载*/
    out << (int)data.type << data.text << data.img << data.urls << data.info;
    return out;
}
QDataStream &operator>>(QDataStream &in, CopyableData &data){
    /*流操作符重载*/
    int typeInt;
    in >> typeInt >> data.text >> data.img >> data.urls >> data.info;
    data.setType(static_cast<DataType>(typeInt));
    return in;
}