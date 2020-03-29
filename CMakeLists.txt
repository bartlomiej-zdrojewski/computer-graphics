cmake_minimum_required(VERSION 3.5)
project(computer-graphics)

set(CMAKE_CXX_STANDARD 11)

file(GLOB SOURCE_FILES "*.h" "*.cpp")
add_executable(computer-graphics ${SOURCE_FILES})

find_package(SFML 2.5 COMPONENTS graphics REQUIRED)
target_link_libraries(computer-graphics sfml-graphics)
