add_subdirectory(configwindow)
add_subdirectory(lyricswindow)
add_subdirectory(mainwindow)
add_subdirectory(playlistwindow)

set(TP_UI_FILES

    tp_menu.h
    tp_menu.cpp

    tp_pushbutton.h
    tp_pushbutton.cpp

    tp_systemtrayicon.h
    tp_systemtrayicon.cpp

    tp_titlebar.h
    tp_titlebar.cpp
)

add_library(TP_ui ${TP_UI_FILES})
add_library(TP::ui ALIAS TP_ui)

target_include_directories(TP_ui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(TP_ui PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    )
