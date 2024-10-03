cmake_minimum_required(VERSION 3.5)
project(MessageBoard)

set(CMAKE_CXX_STANDARD 11)

include_directories(${CMAKE_SOURCE_DIR}/Crow/include)
include_directories(${CMAKE_SOURCE_DIR}/asio-1.18.1/include)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} pthread)
