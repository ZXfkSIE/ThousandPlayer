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

# Integrate images
qt6_add_resources(${PROJECT_NAME} "image"
    PREFIX "/image"
    BASE "${PROJECT_SOURCE_DIR}/image"
    FILES ${TP_IMAGE_FILES}
    )

# integrate markdown files
qt6_add_resources(${PROJECT_NAME} "markdown"
    PREFIX "/markdown"
    BASE "${PROJECT_SOURCE_DIR}/markdown"
    FILES ${TP_MARKDOWN_FILES}
    )

# Integrate translation files
list(GET TP_QM_FILES 0 TP_FIRST_QM_FILE)
cmake_path(GET TP_FIRST_QM_FILE PARENT_PATH TP_QM_PATH)
qt6_add_resources(${PROJECT_NAME} "i18n"
    PREFIX "/i18n"
    BASE ${TP_QM_PATH}
    FILES ${TP_QM_FILES})

# For release build for Windows, it is necessary to set WIN32_EXECUTABLE to true
# in order to get rid of the console window.
if( WIN32 AND (CMAKE_BUILD_TYPE STREQUAL "Release") )
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
        )
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
    TP::global
    TP::ui::configwindow
    TP::ui::lyricswindow
    TP::ui::mainwindow
    TP::ui::playlistwindow

    ${CONAN_LIBS}

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

qt6_finalize_target(ThousandPlayer)
