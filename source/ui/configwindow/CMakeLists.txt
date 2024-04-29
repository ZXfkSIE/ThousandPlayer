set(TP_UI_CONFIGWINDOW_FILES
    tp_configwindow.h
    tp_configwindow.cpp
    tp_configwindow.ui
)

add_library(TP_ui_configwindow ${TP_UI_CONFIGWINDOW_FILES})
add_library(TP::ui::configwindow ALIAS TP_ui_configwindow)

target_include_directories(TP_ui_configwindow PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(TP_ui_configwindow PRIVATE TP::global)
