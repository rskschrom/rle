#include <cstdint>
#include <vector>
#include <chrono>
#include <iostream>
#include "rle.h"

// run-length encoding for a binary array
py::array_t<uint32_t> rleBinary(py::array_t<uint8_t> binaryVec)
{

    size_t nelem = binaryVec.size();
    size_t si = 0;
    size_t ei = 0;

    // create numpy arrays
    auto bvUncheck = binaryVec.unchecked<1>();
    auto rl = py::array_t<uint32_t>(size_t(nelem/10));

    py::buffer_info buf = rl.request();
    uint32_t *rlptr = static_cast<uint32_t *>(buf.ptr);
    
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    
    // check for continuous chunks of the same value
    uint8_t curVal = bvUncheck(0);
    size_t nrl = 0;
    
    while(ei<nelem)
    {
        // next element is the same as the current one
        if(bvUncheck(ei)==curVal)
            ei = ei+1;
        // next element is different than the current one
        else
        {
            rlptr[nrl] = ei-si;
            curVal = 1-curVal;
            si = ei;
            nrl++;
        }
    }
    // add last chunk
    rlptr[nrl] = ei-si;
    rl.resize({nrl+1});
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
  
    return rl;
}

// represent rle with uint16 (run lengths all<2^32)
py::array_t<uint16_t> rlCompress16(py::array_t<uint32_t> rl)
{
    size_t nelem = rl.size();
    //uint32_t si = 0;
    //uint32_t ei = 0;

    // create numpy arrays
    auto rlUncheck = rl.unchecked<1>();
    auto rlCompress = py::array_t<uint16_t>(size_t(2*nelem));

    py::buffer_info buf = rlCompress.request();
    uint16_t *rlptr = static_cast<uint16_t *>(buf.ptr);
    size_t ind = 0;
    uint16_t nu16;
    uint16_t remain;

    // check each element for value > 2^16 (implied to also be < 2^32)
    for(size_t i=0; i<nelem; i++)
    {
        // add larger values to new array in pairs with (0, nu16)
        if(rlUncheck(i)>=ui16_max)
        {
            // get the number of uint16 chunks of run and remainder
            nu16 = uint16_t(rlUncheck(i)/ui16_max);
            remain = rlUncheck(i)-nu16*ui16_max;
            
	    // add 0 and number of chunks to array
	    rlptr[ind] = 0;
	    ind++;
	    rlptr[ind] = nu16;
	    ind++;

	    // add 0 + remainder
	    rlptr[ind] = 0;
            ind++;
	    rlptr[ind] = remain;
	    ind++;
	    //std::cout << rlUncheck(i) << " " << nu16 << " " << remain <<std::endl;
	    //std::cout << i << " " << ind << " " << nelem << std::endl;
        }
        // add smaller values to new array
        else{
            rlptr[ind] = uint16_t(rlUncheck(i));
            ind++;
	    }
    }

    rlCompress.resize({ind});
    return rlCompress;
    
}

// complete function to generate a compressed rle array with a header
py::array_t<uint16_t> encode(py::array_t<uint8_t> binaryVec, size_t nx, size_t ny, size_t nz)
{
    // create numpy arrays
    auto bvUncheck = binaryVec.unchecked<1>();
    //auto rl = py::array_t<uint32_t>(size_t(nelem/10));
    
    uint8_t firstVal = bvUncheck(0);
    
    // get encoding array for uint32 dtype and compress to uint16
    py::array_t<uint32_t> rlu32 = rleBinary(binaryVec);
    py::array_t<uint16_t> rlu16 = rlCompress16(rlu32);
    auto rlu16Uncheck = rlu16.unchecked<1>();

    // create output array
    size_t nrun16 = rlu16.size();
    auto encoded = py::array_t<uint16_t>(size_t(nrun16+4));
    py::buffer_info buf = encoded.request();
    uint16_t *eptr = static_cast<uint16_t *>(buf.ptr);

    // add header to array
    eptr[0] = uint16_t(nx);
    eptr[1] = uint16_t(ny);
    eptr[2] = uint16_t(nz);
    eptr[3] = uint16_t(firstVal);

    // copy uint16 run lengths
    for(size_t i; i<nrun16; i++)
    {
        eptr[i+4] = rlu16Uncheck(i);
    }

    return encoded;
}

// decode compressed rle array
py::array_t<uint8_t> decode(py::array_t<uint16_t> encoded)
{
    size_t nelem = encoded.size();
    size_t si = 0;
    size_t ecount = 0;
    size_t rl, chrun, nchunk, remain;

    // create numpy arrays
    auto encUncheck = encoded.unchecked<1>();

    // read 3d dimensions and create flattened decoded array
    uint16_t nx = encUncheck(0);
    uint16_t ny = encUncheck(1);
    uint16_t nz = encUncheck(2);
    uint8_t curVal = encUncheck(3);
    
    auto decoded = py::array_t<uint8_t>(size_t(nx*ny*nz));
    py::buffer_info buf = decoded.request();
    uint8_t *dptr = static_cast<uint8_t *>(buf.ptr);

    std::cout << nx << " " << ny << " " << nz << std::endl;
    // loop through compressed rle array and generate decoded array
    while(ecount<nelem-4)
    {
        // for runs < uint16_max
        rl = encUncheck(ecount+4);
        if (rl>0)
        {
            //std::cout << si << " " << si+rl << std::endl;
            
            for(size_t j = 0; j<rl; j++)
            {
                dptr[si] = curVal;
                si++;
            }

            // reverse value of curVal
            curVal = 1-curVal;
            //si = si+rl;
        }

        // for runs >= uint16_max
        else
        {
            // get the number of uint16 chunks
            ecount++;
            nchunk = encUncheck(ecount+4);
            ecount += 2;
            remain = encUncheck(ecount+4);
            chrun = nchunk*ui16_max+remain;

            // add values to array
            for(size_t j = 0; j<chrun; j++)
            {
                dptr[si] = curVal;
                si++;
            }

            // reverse value of curVal
            curVal = 1-curVal;
            //std::cout << rl << " " << si << " " << nx*ny*nz << std::endl;
        }

        ecount++;
        //std::cout << rl << " " << si << " " << nx*ny*nz << std::endl;
    }

    std::cout << rl << " " << si << " " << nx*ny*nz << std::endl;
    //std::cout << decoded.size() << std::endl;

    return decoded;

}
