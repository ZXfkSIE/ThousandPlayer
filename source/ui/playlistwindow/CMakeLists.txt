set(TP_UI_PLAYLISTWINDOW_FILES
    tp_filelisttableview.h
    tp_filelisttableview.cpp
    
    tp_listofplayliststableview.h
    tp_listofplayliststableview.cpp
    
    tp_playlistbottomframe.h
    tp_playlistbottomframe.cpp
    
    tp_playlistcontainer.h
    tp_playlistcontainer.cpp
    
    tp_playlistwindow.h
    tp_playlistwindow.cpp
    tp_playlistwindow.ui
)

add_library(TP_ui_playlistwindow ${TP_UI_PLAYLISTWINDOW_FILES})
add_library(TP::ui::playlistwindow ALIAS TP_ui_playlistwindow)

target_include_directories(TP_ui_playlistwindow PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(TP_ui_playlistwindow PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    TP::ui
    )
