set(TP_UI_PLAYLISTWINDOW_FILES

    tp_filelistwidget.h
    tp_filelistwidget.cpp

    tp_replaygainscanprogress.h
    tp_replaygainscanprogress.cpp
    tp_replaygainscanprogress.ui

    tp_runnable_filereader.h
    tp_runnable_filereader.cpp

    tp_runnable_replaygainscanner.h
    tp_runnable_replaygainscanner.cpp

    tp_filesearchdialog.h
    tp_filesearchdialog.cpp
    tp_filesearchdialog.ui

    tp_playlistswidget.h
    tp_playlistswidget.cpp
    
    tp_playlistbottomframe.h
    tp_playlistbottomframe.cpp
    
    tp_playlistcontainer.h
    tp_playlistcontainer.cpp
    
    tp_playlistwindow.h
    tp_playlistwindow.cpp
    tp_playlistwindow.ui
    
    tp_progressdialog.h
    tp_progressdialog.cpp
)

add_library(TP_ui_playlistwindow ${TP_UI_PLAYLISTWINDOW_FILES})
add_library(TP::ui::playlistwindow ALIAS TP_ui_playlistwindow)

target_include_directories(TP_ui_playlistwindow PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(TP_ui_playlistwindow PRIVATE
    TP::global

    TP::ui

    Qt6::Concurrent
    )
