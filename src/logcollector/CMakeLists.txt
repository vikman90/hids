include_directories(include)
include_directories(../agent/include)

file(GLOB sources src/*.cpp)

add_library(logcollector STATIC ${sources})

target_link_libraries(logcollector PRIVATE logger)
target_link_libraries(logcollector PRIVATE config)
target_link_libraries(logcollector PRIVATE cjson)

target_include_directories(logcollector PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
