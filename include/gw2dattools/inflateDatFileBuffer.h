#ifndef GW2DATTOOLS_COMPRESSION_INFLATEDATFILEBUFFER_H
#define GW2DATTOOLS_COMPRESSION_INFLATEDATFILEBUFFER_H

#include <cstdint>
#include <string>
#include <stdexcept>

namespace gw2dt
{
    namespace compression
    {

        /** @Inputs:
         *    - input_size: Size of the input buffer
         *    - input_data: Pointer to the buffer to inflate
         *    - output_data_size: if the value is 0 then we decode everything
         *                    else we decode until we reach the io_outputSize
         *    - output_data: Optional output buffer, in case you provide this buffer,
         *                   output_data_size shall be inferior or equal to the size of this buffer
         *  @Outputs:
         *    - output_data_size: actual size of the outputBuffer
         *  @Return:
         *    - Pointer to the outputBuffer, nullptr if it failed
         *  @Throws:
         *    - gw2dt::std::runtime_error or std::exception in case of error
         */

        uint8_t* inflate_dat_file_buffer(uint32_t input_size, const uint8_t* input_data, uint32_t& output_data_size);

    }
}

#endif // GW2DATTOOLS_COMPRESSION_INFLATEDATFILEBUFFER_H
