set(TP_UI_MAINWINDOW_FILES
    tp_mainwindow.h
    tp_mainwindow.cpp
    tp_mainwindow.ui
)

add_library(TP_ui_mainwindow ${TP_UI_MAINWINDOW_FILES})
add_library(TP::ui::mainwindow ALIAS TP_ui_mainwindow)

target_include_directories(TP_ui_mainwindow PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(TP_ui_mainwindow PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    TP::ui
    )
