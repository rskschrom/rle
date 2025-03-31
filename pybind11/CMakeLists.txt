find_package(Python COMPONENTS Interpreter Development)
set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 CONFIG REQUIRED)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH)

pybind11_add_module(_rle _rle.cpp)
target_link_libraries(_rle PRIVATE utils pybind11::pybind11)
install(TARGETS _rle LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/../)
