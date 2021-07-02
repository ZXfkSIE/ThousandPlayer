enable_testing(true)

add_executable(TP_test_ui tp_test_ui.cpp)
add_test(NAME TP_test_ui COMMAND TP_test_ui)

target_link_libraries(TP_test_ui PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Test
    )
