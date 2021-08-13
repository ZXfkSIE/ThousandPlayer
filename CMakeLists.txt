cmake_minimum_required(VERSION 3.19)

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    message(FATAL "[ThousandPlayer] Sorry, macOS is currently not supported.")
endif()

project(ThousandPlayer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
# In Windows, use Conan to setup 3rd packages
    message(STATUS "[ThousandPlayer] Windows build environment detected.")
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup()
    message(STATUS "[ThousandPlayer] Conan packages loaded: ${CONAN_LIBS}")

elseif(UNIX)
# In *nix, just try to find it
    message(STATUS "[ThousandPlayer] *nix build environment detected.")
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(TAGLIB REQUIRED taglib)
endif()

find_package(Qt6 6.2...6.114514 COMPONENTS
    Widgets
    Multimedia
    LinguistTools
    Test
    REQUIRED)

set(TP_TS_FILES
    ${PROJECT_SOURCE_DIR}/translation/ThousandPlayer_zh_CN.ts
    )

file(GLOB TP_IMAGE_FILES
    ${PROJECT_SOURCE_DIR}/image/*.svg
    )

add_subdirectory(source)


