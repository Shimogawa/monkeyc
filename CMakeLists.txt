cmake_minimum_required(VERSION 3.28)
project(monkeyc_test C)

set(CMAKE_C_STANDARD 11)

set(HEADERS monkey.h)

set(SOURCES test.c)

add_executable(monkeyc_test ${SOURCES} ${HEADERS})

project(monkeycpp_test CXX)

set(CMAKE_CXX_STANDARD 20)

set(HEADERS monkey.h)

set(SOURCES testcpp.cpp)

add_executable(monkeycpp_test ${SOURCES} ${HEADERS})

enable_testing()

add_test(NAME monkeyc_test COMMAND monkeyc_test)
add_test(NAME monkeycpp_test COMMAND monkeycpp_test)
