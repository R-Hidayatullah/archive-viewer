#ifndef GW2DATTOOLS_COMPRESSION_INFLATETEXTUREFILEBUFFER_H
#define GW2DATTOOLS_COMPRESSION_INFLATETEXTUREFILEBUFFER_H

#include <cstdint>
#include <string>
#include <stdexcept>

namespace gw2dt
{
    namespace compression
    {
        struct AnetImage {
            uint32_t identifier;
            uint32_t format;
            uint16_t width;
            uint16_t height;
        };

        /** @Inputs:
         *    - iinput_size: Size of the input buffer
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

        uint8_t* inflate_texture_file_buffer(uint32_t iinput_size, const uint8_t* input_data, uint32_t& output_data_size, AnetImage& anet_image);

        /** @Inputs:
         *    - iWidth: Width of the texture
         *    - iHeight: Height of the texture
         *    - iFormatFourCc: FourCC describing the format of the data
         *    - iinput_size: Size of the input buffer
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

        uint8_t *inflate_texture_block_buffer(uint16_t iWidth, uint16_t iHeight, uint32_t iFormatFourCc, uint32_t iinput_size, const uint8_t *input_data,
                                              uint32_t &output_data_size, uint8_t *output_data = nullptr);
    }
}

#endif // GW2DATTOOLS_COMPRESSION_INFLATETEXTUREFILEBUFFER_H
