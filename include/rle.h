#include<cmath>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#ifndef RLE_H
#define RLE_H

const uint64_t ui16_max = pow(2, 16);
const uint64_t ui32_max = pow(2, 32);

namespace py = pybind11;

py::array_t<uint32_t> rleBinary(py::array_t<uint8_t> binaryVec);
py::array_t<uint16_t> rlCompress16(py::array_t<uint32_t> rl);
py::array_t<uint16_t> encode(py::array_t<uint8_t> binaryVec, size_t nx, size_t ny, size_t nz);
py::array_t<uint8_t> decode(py::array_t<uint16_t> encode);

#endif