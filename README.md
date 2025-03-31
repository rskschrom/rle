# rle
A simple library to compress 3D binary voxel shapes using run-length encoding. The library is wrapped to python using [pybind11](https://github.com/pybind/pybind11)).

## Installation
To build the library, create a `build` directory in the top-level directory of this repository. Then install pybind11 with ```pip install pybind11[global]```. It might be necessary to first create a virtual environment with python before installing pybind11.

After pybind11 is installed, cd into the build directory and run cmake with ```cmake ..```. Then run the command ```make install``` to build the python library. The built library will be located in the top-level repository directory.
