﻿configure_file(tp_cmakeconfig.h.in tp_cmakeconfig.h @ONLY)

set(TP_GLOBAL_FILES
    tp_cmakeconfig.h

    tp_config.h
    tp_config.cpp

    tp_globalconst.h

    tp_globalenum.h

    tp_globalfunction.h
    tp_globalfunction.cpp

    tp_globalvariable.h
    tp_globalvariable.cpp
)

add_library(TP_global ${TP_GLOBAL_FILES})
add_library(TP::global ALIAS TP_global)

target_include_directories(TP_global PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${taglib_INCLUDE_DIRS}
)

target_link_libraries(TP_global PUBLIC
    taglib::taglib

    Qt6::Widgets
    Qt6::Multimedia
)
