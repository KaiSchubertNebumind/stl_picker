// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause
#ifdef HAS_MODULE_QT_WIDGETS
# include <QApplication>
#endif
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QtQuick3D/qquick3d.h>
#include <QQuick3DObject>
#include <QQuick3DGeometry>
#include <QFile>
#include <QTextStream>
#include <QQuickItem>
#include <QObject>
#include <helper.h>

#include <QtQml/qqmlextensionplugin.h>

// Q_IMPORT_QML_PLUGIN(${PROJECT_NAME}Plugin) <- Plugin class name uses UPPER casing
Q_IMPORT_QML_PLUGIN(DownloadManagerPlugin)

int main(int argc, char *argv[])
{
#ifdef HAS_MODULE_QT_WIDGETS
    QApplication app(argc, argv);
#else
    QGuiApplication app(argc, argv);
#endif
    app.setOrganizationName("The Qt Company");
    app.setOrganizationDomain("qt.io");
    app.setApplicationName("STL picker");

    const auto importUrl = argc > 1 ? QUrl::fromLocalFile(argv[1]) : QUrl{};
    if (importUrl.isValid())
        qDebug() << "Importing" << importUrl;

    QSurfaceFormat::setDefaultFormat(QQuick3D::idealSurfaceFormat(4));

    auto my_helper = std::make_unique<Helper>();

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));

    // QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
    //                  &app, []() { QCoreApplication::exit(-1); },
    //                  Qt::QueuedConnection);
    // engine.loadFromModule("CustomGeometry", "Main");

    engine.load(url);

    if (engine.rootObjects().isEmpty()) {
        qWarning() << "Could not find root object in" << url;
        return -1;
    }

    QObject* topLevel = engine.rootObjects().value(0);
    qWarning() << topLevel;

    QObject::connect(topLevel, SIGNAL(qmlSignal(QVariant)),
                     my_helper.get(), SLOT(cppSlot(QVariant)));

    QQuickWindow* window = qobject_cast<QQuickWindow *>(topLevel);
    qWarning() << window;
    qWarning() << window->objectName();
    QObject* view3D = topLevel->findChild<QObject*>("view3D");
    qWarning() << view3D;
    QObject* importNode = view3D->findChild<QObject*>("importNode");
    qWarning() << importNode;
    QQuick3DObject* importNodeObject = qobject_cast<QQuick3DObject *>(importNode);
    qWarning() << "QQuick3DObject log " << importNodeObject;
    qWarning() << "dump";
    importNodeObject->dumpObjectTree();
    // importNodeObject->dumpObjectInfo();
    qWarning() << "dump";
    QQuick3DGeometry* importNodeGeoObject = qobject_cast<QQuick3DGeometry *>(importNode);
    qWarning() << "QQuick3DGeometry log " << importNodeGeoObject;

    auto children = importNodeObject->childItems();
    qWarning() << "children " << children;


    if (window)
        window->setProperty("importUrl", importUrl);

    return app.exec();
}

/*
bool writeModelToObj(QQuick3DObject* model, const QString& outputFilePath) {
    if (!model) {
        qWarning("Model is null");
        return false;
    }

    // Access the geometry property of the model
    QQuick3DGeometry* geometry = qobject_cast<QQuick3DGeometry*>(model->geometry());
    if (!geometry) {
        qWarning("Failed to get geometry from model");
        return false;
    }

    // Open the output file
    QFile file(outputFilePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning("Could not open file for writing");
        return false;
    }
    QTextStream out(&file);

    // Extract geometry data
    QByteArray vertexBuffer = geometry->vertexData();
    QByteArray indexBuffer = geometry->indexData();
    const auto attributes = geometry->attributes();

    QVector3D* vertices = nullptr;
    QVector3D* normals = nullptr;

    // Iterate over the attributes to find vertices and normals
    for (const auto& attribute : attributes) {
        if (attribute.semantic() == QQuick3DGeometry::PositionSemantic) {
            vertices = reinterpret_cast<QVector3D*>(vertexBuffer.data() + attribute.offset());
        }
        if (attribute.semantic() == QQuick3DGeometry::NormalSemantic) {
            normals = reinterpret_cast<QVector3D*>(vertexBuffer.data() + attribute.offset());
        }
    }

    // Write vertices
    int vertexCount = geometry->vertexCount();
    for (int i = 0; i < vertexCount; ++i) {
        if (vertices) {
            out << QString("v %1 %2 %3\n")
            .arg(vertices[i].x())
                .arg(vertices[i].y())
                .arg(vertices[i].z());
        }
    }

    // Write normals
    if (normals) {
        for (int i = 0; i < vertexCount; ++i) {
            out << QString("vn %1 %2 %3\n")
            .arg(normals[i].x())
                .arg(normals[i].y())
                .arg(normals[i].z());
        }
    }

    // Write faces (indices)
    uint32_t* indices = reinterpret_cast<uint32_t*>(indexBuffer.data());
    int indexCount = geometry->indexCount();
    for (int i = 0; i < indexCount; i += 3) {
        out << QString("f %1 %2 %3\n")
        .arg(indices[i] + 1)    // OBJ format uses 1-based indexing
            .arg(indices[i + 1] + 1)
            .arg(indices[i + 2] + 1);
    }

    file.close();
    return true;
}
*/

