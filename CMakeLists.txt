cmake_minimum_required(VERSION 4.2)
project(Funge98)

set(CMAKE_CXX_STANDARD 26)

set(BOOST_ROOT "C:/Users/skyma/.vcpkg-clion/vcpkg/installed/x64-mingw-dynamic")
find_package(Boost REQUIRED COMPONENTS program_options)

add_executable(funge98 interpreter.cpp)
target_link_libraries(funge98 PRIVATE Boost::program_options)