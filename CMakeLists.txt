cmake_minimum_required(VERSION 4.2)
project(Funge98)

set(CMAKE_CXX_STANDARD 26)

add_executable(funge98 src/interpreter.cpp
        src/world/pointer.hpp
        src/world/pointer.cpp
        src/world/stack.cpp
        src/world/stack.hpp
        src/world/vector.hpp
        src/world/vector.cpp
        src/world/world.hpp
        src/world/world.cpp)