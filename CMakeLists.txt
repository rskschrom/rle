cmake_minimum_required(VERSION 3.28)
project("Utils")

cmake_policy(SET CMP0135 NEW)
set(CMAKE_CXX_STANDARD 17)
enable_language(CXX)

set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/../install)
set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib)
set(CMAKE_CXX_FLAGS "-O3 -Wall -fPIC")

# includes
include_directories(${CMAKE_SOURCE_DIR}/include)

# project dirs
add_subdirectory(src)
add_subdirectory(pybind11)
