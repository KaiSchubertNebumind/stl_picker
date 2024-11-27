# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

cmake_minimum_required(VERSION 3.16)
project(runtimeloader LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick Quick3D Widgets)

qt_add_executable(runtimeloader
    main.cpp
)

set_target_properties(runtimeloader PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(runtimeloader PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Quick
    Qt::Quick3D
)

if(TARGET Qt::Widgets)
    target_compile_definitions(runtimeloader PUBLIC
        HAS_MODULE_QT_WIDGETS
    )

    target_link_libraries(runtimeloader PUBLIC
        Qt::Widgets
    )
endif()

add_subdirectory(plugin_custom_geometry)

# Qt6: Generates an additional static library called ${PROJECT_NAME}plugin. <- Plugin cmake project name uses LOWER casing
target_link_libraries(
    runtimeloader
    PRIVATE
    Qt6::Quick
    DownloadManager
)

qt_add_qml_module(runtimeloader
    URI Example
    VERSION 1.0
    QML_FILES
        main.qml
    NO_RESOURCE_TARGET_PATH
    SOURCES
        helper.h helper.cpp
    IMPORTS
        QtQuick3D
)

qt_add_qml_module(customgeometry
    URI CustomGeometry
    VERSION 1.0
    QML_FILES
    SOURCES
        custom_geometry.cpp custom_geometry.h
    RESOURCES qt_logo_rect.png
    RESOURCE_PREFIX "/qt/qml"
    IMPORTS
        QtQuick3D
        QtGui
)

install(TARGETS runtimeloader
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_qml_app_script(
    TARGET runtimeloader
    OUTPUT_SCRIPT deploy_script
    MACOS_BUNDLE_POST_BUILD
    NO_UNSUPPORTED_PLATFORM_ERROR
    DEPLOY_USER_QML_MODULES_ON_UNSUPPORTED_PLATFORM
)
install(SCRIPT ${deploy_script})
