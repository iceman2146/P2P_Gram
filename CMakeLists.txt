cmake_minimum_required(VERSION 3.26.0)
project(boostchat VERSION 0.1.0 LANGUAGES C CXX)
find_package(Boost COMPONENTS system thread chrono REQUIRED)
find_package(Threads REQUIRED)

include_directories(SYSTEM ${Boost_INCLUDE_DIR})
include(CTest)
enable_testing()

add_subdirectory(client)
add_subdirectory(server)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)