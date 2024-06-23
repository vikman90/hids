cmake_minimum_required(VERSION 3.10)

# Set the project name
project(HIDS VERSION 0.2)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(LOG_PATH "agent.log")
set(BUFFER_SIZE 4096)

add_subdirectory(src)
