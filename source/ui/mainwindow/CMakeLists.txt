set(TP_UI_MAINWINDOW_FILES

    tp_audioinfolabel.h
    tp_audioinfolabel.cpp

    tp_coverlabel.h
    tp_coverlabel.cpp

    tp_coverviewer.h
    tp_coverviewer.cpp
    tp_coverviewer.ui

    tp_mainwindow.h
    tp_mainwindow.cpp
    tp_mainwindow.ui

    tp_timeslider.h
    tp_timeslider.cpp

    tp_timesliderproxystyle.h
    tp_timesliderproxystyle.cpp

    tp_volumeicon.h
    tp_volumeicon.cpp
)

add_library(TP_ui_mainwindow ${TP_UI_MAINWINDOW_FILES})
add_library(TP::ui::mainwindow ALIAS TP_ui_mainwindow)

target_include_directories(TP_ui_mainwindow PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(TP_ui_mainwindow PRIVATE
    TP::global

    TP::ui
    )
