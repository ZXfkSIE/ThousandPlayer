set(TP_UI_PLAYLISTWINDOW_FILES

    tp_filelistwidget.h
    tp_filelistwidget.cpp

    tp_playlistswidget.h
    tp_playlistswidget.cpp
    
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
if(UNIX)
    target_include_directories(TP_ui_playlistwindow PRIVATE ${TAGLIB_INCLUDE_DIRS})
    target_link_directories(TP_ui_playlistwindow PRIVATE ${TAGLIB_LIBRARY_DIRS})
    target_compile_options(TP_ui_playlistwindow PRIVATE ${TAGLIB_CFLAGS_OTHER})
endif()

target_link_libraries(TP_ui_playlistwindow PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets

    ${CONAN_LIBS}
    ${TAGLIB_LIBRARIES}

    TP::ui
    )

target_link_libraries(TP_ui_playlistwindow PUBLIC
    TP::global
    )
