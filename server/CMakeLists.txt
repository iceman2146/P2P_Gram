cmake_minimum_required(VERSION 3.26.0)
file(GLOB_RECURSE SOURCE_FILES src/*.cpp)
add_executable(server ${SOURCE_FILES})

set_target_properties(server PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON)

target_link_libraries(server
        ${Boost_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        ws2_32 wsock32)
target_include_directories(server PRIVATE ${Boost_INCLUDE_DIR})



