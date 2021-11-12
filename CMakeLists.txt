cmake_minimum_required(VERSION 3.21)

project(XMakeProjectManager CXX)

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

add_qtc_plugin(${PROJECT_NAME}
    PLUGIN_DEPENDS
	QtCreator::Core
	QtCreator::CppEditor
	QtCreator::TextEditor
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
    SOURCES
	${source_files}

	# ui files
	${ui_files}

	# resources
	resources.qrc
)

target_include_directories(${PROJECT_NAME} PRIVATE src/)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
