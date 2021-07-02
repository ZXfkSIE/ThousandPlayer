add_subdirectory(ui)

add_subdirectory(test)

set(TP_SOURCE_FILES
    main.cpp
    ${TP_TS_FILES}
)

qt_create_translation(TP_QM_FILES ${CMAKE_SOURCE_DIR} ${TP_TS_FILES})

qt_add_executable(ThousandPlayer
    MANUAL_FINALIZATION
    ${TP_QM_FILES}
    ${TP_SOURCE_FILES}
)

target_link_libraries(ThousandPlayer PRIVATE
    TP::ui

    Qt${QT_VERSION_MAJOR}::Widgets
    )

#[[
set_target_properties(ThousandPlayer PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)
]]#

qt_finalize_executable(ThousandPlayer)
