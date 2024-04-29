set(TP_UI_LYRICSWINDOW_FILES
    tp_lyricscontainer.h
    tp_lyricscontainer.cpp

    tp_lyricseditor.h
    tp_lyricseditor.cpp
    tp_lyricseditor.ui

    tp_lyricsstackedwidget.h
    tp_lyricsstackedwidget.cpp

    tp_lyricsviewer.h
    tp_lyricsviewer.cpp

    tp_lyricswindow.h
    tp_lyricswindow.cpp
    tp_lyricswindow.ui
)

add_library(TP_ui_lyricswindow ${TP_UI_LYRICSWINDOW_FILES})
add_library(TP::ui::lyricswindow ALIAS TP_ui_lyricswindow)

target_include_directories(TP_ui_lyricswindow PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(TP_ui_lyricswindow PRIVATE
    TP::global

    TP::ui
    )
