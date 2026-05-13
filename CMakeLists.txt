
cmake_minimum_required(VERSION 3.16)
project(ecims CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

file(GLOB_RECURSE SOURCES src/*.cpp)

add_executable(ecims ${SOURCES})

# Enable all warnings
if(MSVC)
    target_compile_options(ecims PRIVATE /W4)
else()
    target_compile_options(ecims PRIVATE -Wall -Wextra -Wpedantic)
endif()

# On Linux/Mac link stdc++fs for <filesystem>
if(UNIX)
    target_link_libraries(ecims stdc++fs)
endif()
