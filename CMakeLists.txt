cmake_minimum_required(VERSION 3.10)

# Set the project name
project(HIDS VERSION 0.2)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(LOG_PATH "agent.log")
set(BUFFER_SIZE 4096)
set(CONFIG_PATH "etc/agent.yaml")
set(FIM_SIZE_LIMIT 1073741824)
set(FIM_MAX_FILES 100000)

add_subdirectory(src)
