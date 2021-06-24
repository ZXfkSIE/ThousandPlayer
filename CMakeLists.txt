cmake_minimum_required(VERSION 3.5)

project(ThousandPlayer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 COMPONENTS Widgets LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools REQUIRED)

set(TS_FILES ThousandPlayer_zh_CN.ts)

set(PROJECT_SOURCES
        main.cpp
        tp_mainplaywindow.cpp
        tp_mainplaywindow.h
        tp_mainplaywindow.ui
        ${TS_FILES}
)

qt_add_executable(ThousandPlayer
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

target_link_libraries(ThousandPlayer PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

#[[
set_target_properties(ThousandPlayer PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)
]]#

qt_finalize_executable(ThousandPlayer)
