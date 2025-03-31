# rle
A simple library to compress 3D binary voxel shapes using run-length encoding. The library is wrapped to python using [pybind11](https://github.com/pybind/pybind11).

## Installation
To build the library, create a `build` directory in the top-level directory of this repository. Then install pybind11 with ```pip install pybind11[global]```. It might be necessary to first create a virtual environment with python before installing pybind11.

After pybind11 is installed, cd into the build directory and run cmake with ```cmake ..```. Then run the command ```make install``` to build the python library. The built library will be located in the top-level repository directory.

## Usage
There are two main functions associated with this library: the `encode()` function and the `decode()` function that encode and decode, respectively, the particle voxel information. The particle voxel grid is a 3D cuboid grid with dimension of `(nx,ny,nz)`. Each value in the array is either a 1 or 0; values of 1 indicate a voxel is present and values of 0 are void voxels.

To encode a binary voxel grid, first flatten the 3D array into a 1D array. The compression is likely to be most efficient if the order of flatten corresponds to the order of particle dimensions along each cartesian axis. Starting with the 3D voxel grid, we have

```python
# the imports needed
from _rle.rle import encode, decode
import numpy as np

# voxel_3d is a 3D array with dimension (nx,ny,nz)
voxel_1d = voxel_grid.flatten()
'''.

Encode the voxel array with

```python
enc = encode(voxel_1d, nx, ny, nz)
'''.

We can write the encoded data to a file with

```python
enc.tofile('encoded.bin')
'''.

The data are stored as `uint16` values, with alternating values corresponding to runs of 0 or 1. The header of the file contains information about the array dimensions and the first run value. For runs longer than `2^16-1`, four consecutive values `uint16` values are stored. The first is a 0 to indicate the start of a long run. The second value is the number of `2^16` consecutive runs of a given value (0 or 1). The third value is another 0 and the fourth value is the remainder of the long run.

To decode the encoded array, we use

```python
dec = decode(enc)
dec = np.reshape(dec, (nx,ny,nz))
'''.

