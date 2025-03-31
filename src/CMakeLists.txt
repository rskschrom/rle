set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 CONFIG REQUIRED)

add_library(utils rle.cpp)
target_link_libraries(utils PUBLIC pybind11::pybind11)
install(TARGETS utils LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX})
