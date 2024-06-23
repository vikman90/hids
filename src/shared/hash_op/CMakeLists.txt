file(GLOB sources src/*.cpp)

add_library(hash_op STATIC ${sources})

target_link_libraries(hash_op PRIVATE config)
target_link_libraries(hash_op PRIVATE crypto)

target_include_directories(hash_op PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
