add_subdirectory(config)
add_subdirectory(global)
add_subdirectory(ui)

set(TP_SOURCE_FILES
    main.cpp

    tp_mainclass.h
    tp_mainclass.cpp
)

qt6_create_translation(TP_QM_FILES
    ${PROJECT_SOURCE_DIR}/source
    ${TP_TS_FILES}
    )

qt6_add_executable(${PROJECT_NAME}
    MANUAL_FINALIZATION
    ${TP_SOURCE_FILES}
    ${TP_TS_FILES}
    )

qt6_add_resources(${PROJECT_NAME} "image"
    PREFIX "/image"
    BASE "${PROJECT_SOURCE_DIR}/image"
    FILES ${TP_IMAGE_FILES}
    )


list(GET TP_QM_FILES 0 TP_FIRST_QM_FILE)
cmake_path(GET TP_FIRST_QM_FILE PARENT_PATH TP_QM_PATH)
qt6_add_resources(${PROJECT_NAME} "i18n"
    PREFIX "/i18n"
    BASE ${TP_QM_PATH}
    FILES ${TP_QM_FILES})

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
