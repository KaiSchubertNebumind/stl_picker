# Qt6 CMake QML Plugin example project
This is an example project for easy QML plugin creation. This example creates a simple c++ based download manager to easily download files from a remote server to a certain local path. If you want to create your own version, simply clone this project into your existing cmake based project and add it via:

## Add to your project:
In your CMakeLists.txt
``` cmake
add_subdirectory(MyQMLExample)

# Qt6: Generates an additional static library called ${PROJECT_NAME}plugin. <- Plugin cmake project name uses LOWER casing 
target_link_libraries(MyProject PRIVATE Qt6::Quick MyQMLExampleplugin)
```

In your main.cpp
``` cpp
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml/qqmlextensionplugin.h>

// Q_IMPORT_QML_PLUGIN(${PROJECT_NAME}Plugin) <- Plugin class name uses UPPER casing
Q_IMPORT_QML_PLUGIN(MyQMLExamplePlugin)

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    // Start with qrc:/qt/qml for qtp0001
    // https://doc.qt.io/qt-6/qt-cmake-policy-qtp0001.html
    // The first subfolder is the libraryName followed by the regular
    // folder strucutre:     LibararyName/Subfolder
    const QUrl url(u"qrc:/qt/qml/MyQMLExample/MyQMLExample/TestMain.qml"_qs);
    engine.load(url);

    return app.exec();
}

```

Then change the project cmake variable in line 3 of the CMakeLists.txt to your pluginName. Additionaly you can set the TestProject variable to OFF in line 20.

##  Resources
- Qt docs: [https://doc-snapshots.qt.io/qt6-dev/qt-add-qml-module.html](https://doc-snapshots.qt.io/qt6-dev/qt-add-qml-module.html)
- Qt blog post: [https://www.qt.io/blog/qml-modules-in-qt-6.2](https://www.qt.io/blog/qml-modules-in-qt-6.2)

License: MIT
