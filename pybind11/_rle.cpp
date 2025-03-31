#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "rle.h"

namespace py = pybind11;

PYBIND11_MODULE(_rle, m){
    auto m_rle = m.def_submodule("rle", "Submodule for run-length encoding");
    //m_rle.def("rleBinary", py::overload_cast<std::vector<uint8_t>>(&rleBinary));
    m_rle.def("rleBinary", &rleBinary);
    m_rle.def("rlCompress16", &rlCompress16);
    m_rle.def("encode", &encode);
    m_rle.def("decode", &decode);
    m.doc() = "";

}
