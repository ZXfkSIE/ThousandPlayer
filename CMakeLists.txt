cmake_minimum_required(VERSION 3.19)

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    message(FATAL "Sorry, macOS is currently not supported.")
endif()

project(ThousandPlayer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.2...6.114514 COMPONENTS
    Widgets
    Multimedia
    LinguistTools
    Test
    REQUIRED)

set(TP_TS_FILES
    ${PROJECT_SOURCE_DIR}/translation/ThousandPlayer_zh_CN.ts
    )

set(TP_IMAGE_FILES
    ${PROJECT_SOURCE_DIR}/image/icon_Minimize.svg
    ${PROJECT_SOURCE_DIR}/image/icon_Expand.svg
    ${PROJECT_SOURCE_DIR}/image/icon_Exit.svg
    ${PROJECT_SOURCE_DIR}/image/icon_Volume.svg
    ${PROJECT_SOURCE_DIR}/image/icon_Mute.svg
    ${PROJECT_SOURCE_DIR}/image/MusicalNote.svg
    )

add_subdirectory(source)


