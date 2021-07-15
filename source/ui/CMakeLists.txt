set(TP_UI_FILES
    tp_mainwindow.h
    tp_mainwindow.cpp
    tp_mainwindow.ui

    tp_playlist.h
    tp_playlist.cpp
    tp_playlist.ui

    tp_playlistcontainer.h
    tp_playlistcontainer.cpp

    tp_titlebar.h
    tp_titlebar.cpp
)

add_library(TP_ui ${TP_UI_FILES})
add_library(TP::ui ALIAS TP_ui)

qt6_add_resources(TP_ui "image"
    PREFIX "/image"
    BASE "${PROJECT_SOURCE_DIR}/image"
    FILES ${TP_IMAGE_FILES}
    )

target_include_directories(TP_ui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(TP_ui PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
