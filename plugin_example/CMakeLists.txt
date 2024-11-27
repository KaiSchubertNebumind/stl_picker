cmake_minimum_required(VERSION 3.20.0)

project(
    DownloadManager
    VERSION 0.1
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(
    Qt6 6.5
    COMPONENTS
                Core
                Quick
                Test
    REQUIRED)

qt_standard_project_setup(REQUIRES 6.5)


# Because this is a plugin, we need this for testing and development. This can be disabled when using the plugin in your project directly.
option(TestProject "Builds TextProject" ON)

set(QML
    # cmake-format: sortable
    ${PROJECT_NAME}/TestMain.qml # Used only by our TestProject below!
    ${PROJECT_NAME}/DownloadManagerControls.qml)

set(QML_PLUGIN_SOURCES # cmake-format: sortable
                       ${PROJECT_NAME}/DownloadManager.cpp)

set(QML_PLUGIN_HEADER # cmake-format: sortable
                      ${PROJECT_NAME}/DownloadManager.h)

qt_add_library(${PROJECT_NAME} STATIC)
qt_add_qml_module(
    ${PROJECT_NAME}
    URI
    ${PROJECT_NAME}
    OUTPUT_DIRECTORY
    "${CMAKE_BINARY_DIR}/${PROJECT_NAME}"
    VERSION
    1.0
    QML_FILES
    ${QML}
    SOURCES
    ${QML_PLUGIN_SOURCES}
    ${QML_PLUGIN_HEADER})

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_NAME})

if(${TestProject})
    qt_add_executable(TestProject Test/TestMain.cpp)
    # Qt6: Generates an additional static library called ${PROJECT_NAME}plugin. <- Plugin LOWER casing All users of this must also call
    # Q_IMPORT_QML_PLUGIN in the main.cpp between the includes and main:
    #
    # #include <QtQml/qqmlextensionplugin.h> Q_IMPORT_QML_PLUGIN(${PROJECT_NAME}Plugin) <- Plugin UPPER casing
    #
    # See: https://www.qt.io/blog/qml-modules-in-qt-6.2
    target_link_libraries(TestProject PRIVATE Qt6::Quick ${PROJECT_NAME}plugin)
    target_compile_definitions(TestProject PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
endif()
