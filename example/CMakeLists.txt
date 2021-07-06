cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 20)
set(BINARY xalwart-example-server)
set(CMAKE_CXX_FLAGS "-pthread")

project(${BINARY})

file(GLOB SOURCES true *.cpp)
add_executable(${BINARY} ${SOURCES} main.cpp)
target_link_libraries(${BINARY} PUBLIC xalwart.core-0.0.1)
target_link_libraries(${BINARY} PUBLIC xalwart.server-0.0.1)
