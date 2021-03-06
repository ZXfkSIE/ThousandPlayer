﻿configure_file(tp_cmakeconfig.h.in tp_cmakeconfig.h @ONLY)

set(TP_GLOBAL_FILES
    tp_cmakeconfig.h

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
)
if(UNIX)
    target_include_directories(TP_global PRIVATE ${TAGLIB_INCLUDE_DIRS})
    target_link_directories(TP_global PRIVATE ${TAGLIB_LIBRARY_DIRS})
    target_compile_options(TP_global PRIVATE ${TAGLIB_CFLAGS_OTHER})
endif()

target_link_libraries(TP_global PUBLIC
    TP::config

    ${CONAN_LIBS}
    ${TAGLIB_LIBRARIES}

    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Multimedia
)
