set(TP_GLOBAL_FILES
    tp_globalconst.h

    tp_globalenum.h

    tp_globalfunction.h
    tp_globalfunction.cpp

    tp_globalvariable.h
    tp_globalvariable.cpp
)

add_library(TP_global ${TP_GLOBAL_FILES})
add_library(TP::global ALIAS TP_global)

target_include_directories(TP_global PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(TP_global PUBLIC
    TP::config

    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Multimedia
    )
