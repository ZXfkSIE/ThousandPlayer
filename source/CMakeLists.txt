add_subdirectory(config)
add_subdirectory(global)
add_subdirectory(ui)

add_subdirectory(test)

set(TP_SOURCE_FILES
    main.cpp

    tp_mainclass.h
    tp_mainclass.cpp

    ${TP_TS_FILES}
)

qt6_create_translation(TP_QM_FILES ${CMAKE_SOURCE_DIR} ${TP_TS_FILES})

qt6_add_executable(${PROJECT_NAME}
    MANUAL_FINALIZATION
    ${TP_QM_FILES}
    ${TP_SOURCE_FILES}
)

qt6_add_resources(${PROJECT_NAME} "image"
    PREFIX "/image"
    BASE "${PROJECT_SOURCE_DIR}/image"
    FILES ${TP_IMAGE_FILES}
    )

if(UNIX)
    target_include_directories(${PROJECT_NAME} PRIVATE ${TAGLIB_INCLUDE_DIRS})
    target_link_directories(${PROJECT_NAME} PRIVATE ${TAGLIB_LIBRARY_DIRS})
    target_compile_options(${PROJECT_NAME} PRIVATE ${TAGLIB_CFLAGS_OTHER})
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
    TP::global
    TP::ui::configwindow
    TP::ui::mainwindow
    TP::ui::playlistwindow

    ${CONAN_LIBS}
    ${TAGLIB_LIBRARIES}

    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Multimedia
    )

#[[
set_target_properties(ThousandPlayer PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)
]]#

qt_finalize_executable(ThousandPlayer)
