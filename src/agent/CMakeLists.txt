include_directories(include)

find_package(cJSON REQUIRED)
find_package(OpenSSL REQUIRED)

file(GLOB sources src/*.cpp)

add_executable(agent ${sources})

target_link_libraries(agent PRIVATE config)
target_link_libraries(agent PRIVATE fim)
target_link_libraries(agent PRIVATE logger)
target_link_libraries(agent PRIVATE logcollector)
target_link_libraries(agent PRIVATE yaml)
