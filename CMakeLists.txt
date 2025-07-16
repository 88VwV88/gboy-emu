cmake_minimum_required(VERSION 3.26)
project(GBOY-EMU)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB_RECURSE SRC "src/*.cpp" "src/*.hpp")
message("Found source file: ${SRC}")
add_executable(gboy ${SRC})
target_compile_options(gboy INTERFACE "<$BUILD_INTERFACE:-Wall;-Werror;-Wconversion-O0>")
