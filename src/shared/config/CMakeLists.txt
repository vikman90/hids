configure_file(include/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/config.h)

add_library(config INTERFACE)

target_include_directories(config INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/include)
