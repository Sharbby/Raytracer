cmake_minimum_required(VERSION 3.0.0)
project(graphic VERSION 0.1.0 LANGUAGES C)

include(CTest)
enable_testing()

add_executable(graphic main.c)

target_link_libraries(graphic mingw32)
target_link_libraries(graphic SDL2main)
target_link_libraries(graphic SDL2)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
