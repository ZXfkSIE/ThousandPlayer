set(TP_UI_MAINWINDOW_FILES
    tp_albumcoverlabel.h
    tp_albumcoverlabel.cpp

    tp_mainwindow.h
    tp_mainwindow.cpp
    tp_mainwindow.ui

    tp_timeslider.h
    tp_timeslider.cpp

    tp_visualcontainer.h
    tp_visualcontainer.cpp

    tp_volumeicon.h
    tp_volumeicon.cpp

    tp_volumeslider.h
    tp_volumeslider.cpp
)

add_library(TP_ui_mainwindow ${TP_UI_MAINWINDOW_FILES})
add_library(TP::ui::mainwindow ALIAS TP_ui_mainwindow)

target_include_directories(TP_ui_mainwindow PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
if(UNIX)
    target_include_directories(TP_ui_mainwindow PRIVATE ${TAGLIB_INCLUDE_DIRS})
    target_link_directories(TP_ui_mainwindow PRIVATE ${TAGLIB_LIBRARY_DIRS})
    target_compile_options(TP_ui_mainwindow PRIVATE ${TAGLIB_CFLAGS_OTHER})
endif()

target_link_libraries(TP_ui_mainwindow PRIVATE
    TP::ui

    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Multimedia

    ${CONAN_LIBS}
    ${TAGLIB_LIBRARIES}
    )
target_link_libraries(TP_ui_mainwindow PUBLIC
    TP::global
    )
