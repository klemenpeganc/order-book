include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
)

FetchContent_MakeAvailable(googletest)

project(order-book-kraken-test)
file(GLOB TEST_SOURCES *Test.cpp)
add_executable(${PROJECT_NAME} main.cpp ${TEST_SOURCES})

target_link_libraries(${PROJECT_NAME} GTest::gtest)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)