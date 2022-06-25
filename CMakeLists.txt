cmake_minimum_required(VERSION 3.18)

project(XMakeProjectManager VERSION 7.0.0
                            DESCRIPTION "QtCreator XMake project support"
                            LANGUAGES CXX)

list(APPEND CMAKE_PREFIX_PATH "${QTC_BUILD_PATH}")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(QtCreator COMPONENTS Core ExtensionSystem Utils REQUIRED)
find_package(Qt6 COMPONENTS Core Widgets REQUIRED)

file(GLOB_RECURSE
    source_files
    src/*.cpp
    src/*.hpp
    src/*.inl
)

file(GLOB_RECURSE
    ui_files
    src/*.ui
)

file(GLOB_RECURSE
    assets_files
    assets/*.lua
)

add_qtc_plugin(
    ${PROJECT_NAME}
    PLUGIN_DEPENDS
        QtCreator::Core
        QtCreator::CppEditor
        QtCreator::ProjectExplorer
        QtCreator::QtSupport
    PLUGIN_RECOMMENDS
        QtCreator::Designer
        QtCreator::LangageClient
    DEPENDS
        Qt::Core
        Qt::Widgets
        QtCreator::Core
        QtCreator::ExtensionSystem
        QtCreator::Utils
        QtCreator::QmlJS
    SOURCES
        ${source_files}

        ${ui_files}

        ${assets_files}

        resources.qrc
)

target_include_directories(${PROJECT_NAME} PRIVATE src/)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
