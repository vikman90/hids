include_directories(include)
include_directories(../agent/include)

file(GLOB sources src/*.cpp)

add_library(fim STATIC ${sources})

target_link_libraries(fim PRIVATE config)
target_link_libraries(fim PRIVATE file_op)
target_link_libraries(fim PRIVATE hash_op)
target_link_libraries(fim PRIVATE logger)
target_link_libraries(fim PRIVATE time_op)
target_link_libraries(fim PRIVATE cjson)

target_include_directories(fim PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
