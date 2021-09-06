set(TP_GLOBAL_FILES
    tp_globalconst.h
    tp_globalenum.h
)

add_library(TP_global INTERFACE ${TP_GLOBAL_FILES})
add_library(TP::global ALIAS TP_global)

target_include_directories(TP_global INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
