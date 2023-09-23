cmake_minimum_required(VERSION 3.26)

project(P2P_Gram VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

set (wxWidgets_ROOT_DIR  "C:/vxWidgets")
set (wxWidgets_LIB_DIR  "C:/vxWidgets/lib/gcc810_x64_dll")

find_package(wxWidgets REQUIRED COMPONENTS net core base)

include(${wxWidgets_USE_FILE})
include_directories(include/client)
include_directories(include/server)
include_directories(include/common)
file (GLOB project_GLOB
src/*)
##add_executable( main ${project_GLOB})
add_executable(client ./src/client/client.cpp)

add_executable(server ./src/server/server.cpp)

target_link_libraries(client ${wxWidgets_LIBRARIES})
target_include_directories(client PRIVATE ${wxWidgets_LIBRARIES_DIRS})



target_link_libraries(server ${wxWidgets_LIBRARIES})
target_include_directories(server PRIVATE ${wxWidgets_LIBRARIES_DIRS})


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)