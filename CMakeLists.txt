cmake_minimum_required(VERSION 3.28)
project(monkeyc_test C)

set(CMAKE_C_STANDARD 11)

set(HEADERS monkey.h)

set(SOURCES test.c)

add_executable(monkeyc_test ${SOURCES} ${HEADERS})
