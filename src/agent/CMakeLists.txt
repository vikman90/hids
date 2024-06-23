include_directories(include)

file(GLOB sources src/*.cpp)

add_executable(agent ${sources})
