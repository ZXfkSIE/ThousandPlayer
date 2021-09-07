set(TP_CONFIG_FILES
    tp_config.h
    tp_config.cpp
)


add_library(TP_config ${TP_CONFIG_FILES})
add_library(TP::config ALIAS TP_config)

target_include_directories(TP_config PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(TP_config PRIVATE
    TP::global

    Qt${QT_VERSION_MAJOR}::Widgets
    )
