cmake_minimum_required(VERSION 3.10)
project(order-book-kraken VERSION 1.0)
enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(test)
aux_source_directory(src SOURCES)
set(CMAKE_BINARY_DIR build)
add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_directories(${PROJECT_NAME} PUBLIC include)