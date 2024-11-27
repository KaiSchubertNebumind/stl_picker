#ifndef HELPER_H
#define HELPER_H

#include <QObject>
#include <QQuick3DObject>

class Helper : public QObject
{
    Q_OBJECT
public:
    explicit Helper(QObject *parent = nullptr);
    auto load_scene();

public slots:
    void cppSlot(QObject *item) {
        qDebug() << "Called the C++ slot with item:" << item;

    }
signals:


};

#endif // HELPER_H
