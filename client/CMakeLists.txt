cmake_minimum_required(VERSION 3.26.0)
file(GLOB_RECURSE SOURCE_FILES src/*.cpp)
add_executable(client ${SOURCE_FILES})

set_target_properties(client PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON)

target_link_libraries(client
        ${Boost_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        ws2_32 wsock32)
target_include_directories(client PRIVATE ${Boost_INCLUDE_DIR})



