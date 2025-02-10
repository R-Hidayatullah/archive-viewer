#include "gw2dattools/inflateTextureFileBuffer.h"

#include <cstdlib>
#include <memory.h>

#include "gw2dattools/huffmanTreeUtils.h"

#include <iostream>
#include <vector>

namespace gw2dt
{
	namespace compression
	{


		namespace texture
		{

			struct Format
			{
				uint16_t flag_data;
				uint16_t pixel_size_bits;
			};

			struct FullFormat
			{
				Format format;
				uint32_t pixel_blocks;
				uint32_t bytes_pixel_blocks;
				uint32_t bytes_component;
				bool two_component;

				uint16_t width;
				uint16_t height;
			};

		

			enum FormatFlags
			{
				FF_COLOR = 0x10,                 // Standard color format
				FF_ALPHA = 0x20,                 // Has alpha channel
				FF_DEDUCEDALPHACOMP = 0x40,      // Alpha is deduced from compression
				FF_PLAINCOMP = 0x80,             // Plain compression (single-channel)
				FF_BICOLORCOMP = 0x200,          // Two-channel compression (e.g., normal maps)
				FF_HDR = 0x400,                  // High Dynamic Range (HDR) format
				FF_BPTC = 0x800                  // BC6H/BC7 block compression
			};

			enum CompressionFlags
			{
				CF_DECODE_WHITE_COLOR = 0x01,                      // Used for decoding white color
				CF_DECODE_CONSTANT_ALPHA_FROM4BITS = 0x02,        // Alpha from 4-bit constant
				CF_DECODE_CONSTANT_ALPHA_FROM8BITS = 0x04,        // Alpha from 8-bit constant
				CF_DECODE_PLAIN_COLOR = 0x08,                     // Plain color decoding
				CF_DECODE_BPTC_FLOAT = 0x10,                      // For BC6H (HDR) float decoding
				CF_DECODE_BPTC_UNORM = 0x20                       // For BC7 normalized decoding
			};


			// Static Values
			HuffmanTree huffman_tree_dict;
			Format format_data[11];
			bool static_values_initialized(false);

			void initialize_static_values()
			{
				// Formats
				{
					// DXT1 (S3TC - BC1)
					Format& format_dxt1_data = format_data[0];
					format_dxt1_data.flag_data = FF_COLOR | FF_ALPHA | FF_DEDUCEDALPHACOMP;
					format_dxt1_data.pixel_size_bits = 4;

					// DXT2, DXT3, DXT4 (S3TC - BC2)
					Format& format_dxt2_data = format_data[1];
					format_dxt2_data.flag_data = FF_COLOR | FF_ALPHA | FF_PLAINCOMP;
					format_dxt2_data.pixel_size_bits = 8;

					format_data[2] = format_data[1]; // DXT3 (same as DXT2)
					format_data[3] = format_data[1]; // DXT4 (same as DXT2)

					// DXT5 (S3TC - BC3)
					Format& format_dxt5_data = format_data[4];
					format_dxt5_data.flag_data = FF_COLOR | FF_ALPHA | FF_PLAINCOMP;
					format_dxt5_data.pixel_size_bits = 8;

					// DXTA (BC4 - ATI1, single-channel)
					Format& format_dxt_a_data = format_data[5];
					format_dxt_a_data.flag_data = FF_ALPHA | FF_PLAINCOMP;
					format_dxt_a_data.pixel_size_bits = 4;

					// DXTL (BC4 - ATI1, single-channel grayscale)
					Format& format_dxt_l_data = format_data[6];
					format_dxt_l_data.flag_data = FF_ALPHA | FF_PLAINCOMP;
					format_dxt_l_data.pixel_size_bits = 4;

					// DXTN (BC5 - ATI2, normal maps)
					Format& format_dxt_n_data = format_data[7];
					format_dxt_n_data.flag_data = FF_BICOLORCOMP;
					format_dxt_n_data.pixel_size_bits = 8;

					// 3Dc (BC5 variant, normal maps)
					Format& format_3dcx_data = format_data[8];
					format_3dcx_data.flag_data = FF_BICOLORCOMP;
					format_3dcx_data.pixel_size_bits = 8;

					// BC6H (HDR compressed)
					Format& format_bc6h_data = format_data[9];
					format_bc6h_data.flag_data = FF_COLOR | FF_HDR | FF_BPTC;
					format_bc6h_data.pixel_size_bits = 8;  // BC6H uses 16-bit floats internally but compressed

					// BC7 (High-quality compressed color format)
					Format& format_bc7_data = format_data[10];
					format_bc7_data.flag_data = FF_COLOR | FF_ALPHA | FF_BPTC;
					format_bc7_data.pixel_size_bits = 8;
				}


				int16_t bits_head_data[MAX_CODE_BITS_LENGTH];
				int16_t bits_body_data[MAX_SYMBOL_VALUE];

				// Initialize our workingTabs
				memset(&bits_head_data, 0xFF, MAX_CODE_BITS_LENGTH * sizeof(int16_t));
				memset(&bits_body_data, 0xFF, MAX_SYMBOL_VALUE * sizeof(int16_t));

				fill_working_tab(1, 0x01, &bits_head_data[0], &bits_body_data[0]);

				fill_working_tab(2, 0x12, &bits_head_data[0], &bits_body_data[0]);

				fill_working_tab(6, 0x11, &bits_head_data[0], &bits_body_data[0]);
				fill_working_tab(6, 0x10, &bits_head_data[0], &bits_body_data[0]);
				fill_working_tab(6, 0x0F, &bits_head_data[0], &bits_body_data[0]);
				fill_working_tab(6, 0x0E, &bits_head_data[0], &bits_body_data[0]);
				fill_working_tab(6, 0x0D, &bits_head_data[0], &bits_body_data[0]);
				fill_working_tab(6, 0x0C, &bits_head_data[0], &bits_body_data[0]);
				fill_working_tab(6, 0x0B, &bits_head_data[0], &bits_body_data[0]);
				fill_working_tab(6, 0x0A, &bits_head_data[0], &bits_body_data[0]);
				fill_working_tab(6, 0x09, &bits_head_data[0], &bits_body_data[0]);
				fill_working_tab(6, 0x08, &bits_head_data[0], &bits_body_data[0]);
				fill_working_tab(6, 0x07, &bits_head_data[0], &bits_body_data[0]);
				fill_working_tab(6, 0x06, &bits_head_data[0], &bits_body_data[0]);
				fill_working_tab(6, 0x05, &bits_head_data[0], &bits_body_data[0]);
				fill_working_tab(6, 0x04, &bits_head_data[0], &bits_body_data[0]);
				fill_working_tab(6, 0x03, &bits_head_data[0], &bits_body_data[0]);
				fill_working_tab(6, 0x02, &bits_head_data[0], &bits_body_data[0]);

				return build_huffmantree(huffman_tree_dict, &bits_head_data[0], &bits_body_data[0]);
			}

			Format deduceFormat(uint32_t four_cc_data)
			{
				switch (four_cc_data)
				{
				case 0x31545844: // "DXT1"
					return format_data[0];

				case 0x32545844: // "DXT2"
					return format_data[1];

				case 0x33545844: // "DXT3"
					return format_data[2];

				case 0x34545844: // "DXT4"
					return format_data[3];

				case 0x35545844: // "DXT5"
					return format_data[4];

				case 0x41545844: // "DXTA" (ATI1 / BC4)
					return format_data[5];

				case 0x4C545844: // "DXTL" (Luminance BC4)
					return format_data[6];

				case 0x4E545844: // "DXTN" (ATI2 / BC5)
					return format_data[7];

				case 0x58434433: // "3DCX" (BC5 variant)
					return format_data[8];

				case 0x48364342: // "BC6H" (HDR format)
					return format_data[9];

				case 0x58374342: // "BC7X" (BC7)
					return format_data[10];

				default:
					throw std::runtime_error("Unknown texture format: " + std::to_string(four_cc_data));
				}
			}


			void decode_white_color(State& state_data, std::vector<bool>& alpha_bit_map, std::vector<bool>& color_bit_map, const FullFormat& full_format_data, uint8_t* output_data)
			{
				uint32_t pixel_block_position = 0;

				while (pixel_block_position < full_format_data.pixel_blocks)
				{
					// Reading next code
					uint16_t temp_code = 0;
					read_code(huffman_tree_dict, state_data, temp_code);

					need_bits(state_data, 1);
					uint32_t value_data = read_bits(state_data, 1);
					drop_bits(state_data, 1);

					while (temp_code > 0)
					{
						if (!color_bit_map[pixel_block_position])
						{
							if (value_data)
							{
								*reinterpret_cast<int64_t*>(&(output_data[full_format_data.bytes_pixel_blocks * (pixel_block_position)])) = 0xFFFFFFFFFFFFFFFE;

								alpha_bit_map[pixel_block_position] = true;
								color_bit_map[pixel_block_position] = true;
							}
							--temp_code;
						}
						++pixel_block_position;
					}

					while (pixel_block_position < full_format_data.pixel_blocks && color_bit_map[pixel_block_position])
					{
						++pixel_block_position;
					}
				}
			}

			void decode_constant_alpha_from_4_bits(State& state_data, std::vector<bool>& alpha_bit_map, const FullFormat& full_format_data, uint8_t* output_data)
			{
				need_bits(state_data, 4);
				uint8_t alpha_value_byte = static_cast<uint8_t>(read_bits(state_data, 4));
				drop_bits(state_data, 4);

				uint32_t pixel_block_position = 0;

				uint16_t intermediate_byte = alpha_value_byte | (alpha_value_byte << 4);
				uint32_t interediate_word = intermediate_byte | (intermediate_byte << 8);
				uint64_t intermediate_dword = interediate_word | (interediate_word << 16);
				uint64_t alpha_value = intermediate_dword | (intermediate_dword << 32);
				uint64_t zero_data = 0;

				while (pixel_block_position < full_format_data.pixel_blocks)
				{
					// Reading next code
					uint16_t temp_code = 0;
					read_code(huffman_tree_dict, state_data, temp_code);

					need_bits(state_data, 2);
					uint32_t value_data = read_bits(state_data, 1);
					drop_bits(state_data, 1);

					uint8_t isNotNull = static_cast<uint8_t>(read_bits(state_data, 1));
					if (value_data)
					{
						drop_bits(state_data, 1);
					}
					while (temp_code > 0)
					{
						if (!alpha_bit_map[pixel_block_position])
						{
							if (value_data)
							{
								memcpy(&(output_data[full_format_data.bytes_pixel_blocks * (pixel_block_position)]), isNotNull ? &alpha_value : &zero_data, full_format_data.bytes_component);
								alpha_bit_map[pixel_block_position] = true;
							}
							--temp_code;
						}
						++pixel_block_position;
					}

					while (pixel_block_position < full_format_data.pixel_blocks && alpha_bit_map[pixel_block_position])
					{
						++pixel_block_position;
					}
				}
			}

			void decode_constant_alpha_from_8_bits(State& state_data, std::vector<bool>& alpha_bit_map, const FullFormat& full_format_data, uint8_t* output_data)
			{
				need_bits(state_data, 8);
				uint8_t alpha_value_byte = static_cast<uint8_t>(read_bits(state_data, 8));
				drop_bits(state_data, 8);

				uint32_t pixel_block_position = 0;

				uint64_t alpha_value = alpha_value_byte | (alpha_value_byte << 8);
				uint64_t zero_data = 0;

				while (pixel_block_position < full_format_data.pixel_blocks)
				{
					// Reading next code
					uint16_t temp_code = 0;
					read_code(huffman_tree_dict, state_data, temp_code);

					need_bits(state_data, 2);
					uint32_t value_data = read_bits(state_data, 1);
					drop_bits(state_data, 1);

					uint8_t isNotNull = static_cast<uint8_t>(read_bits(state_data, 1));
					if (value_data)
					{
						drop_bits(state_data, 1);
					}
					while (temp_code > 0)
					{
						if (!alpha_bit_map[pixel_block_position])
						{
							if (value_data)
							{
								memcpy(&(output_data[full_format_data.bytes_pixel_blocks * (pixel_block_position)]), isNotNull ? &alpha_value : &zero_data, full_format_data.bytes_component);
								alpha_bit_map[pixel_block_position] = true;
							}
							--temp_code;
						}
						++pixel_block_position;
					}

					while (pixel_block_position < full_format_data.pixel_blocks && alpha_bit_map[pixel_block_position])
					{
						++pixel_block_position;
					}
				}
			}

			void decode_plain_color(State& state_data, std::vector<bool>& color_bit_map, const FullFormat& full_format_data, uint8_t* output_data)
			{
				need_bits(state_data, 24);
				uint16_t blue_data = static_cast<uint16_t>(read_bits(state_data, 8));
				drop_bits(state_data, 8);
				uint16_t green_data = static_cast<uint16_t>(read_bits(state_data, 8));
				drop_bits(state_data, 8);
				uint16_t red_data = static_cast<uint16_t>(read_bits(state_data, 8));
				drop_bits(state_data, 8);

				// TEMP

				uint8_t temp_red_data_1 = static_cast<uint8_t>((red_data - (red_data >> 5)) >> 3);
				uint8_t temp_blue_data_1 = static_cast<uint8_t>((blue_data - (blue_data >> 5)) >> 3);

				uint16_t temp_green_data_1 = (green_data - (green_data >> 6)) >> 2;

				uint8_t temp_red_data_2 = (temp_red_data_1 << 3) + (temp_red_data_1 >> 2);
				uint8_t temp_blue_data_2 = (temp_blue_data_1 << 3) + (temp_blue_data_1 >> 2);

				uint16_t temp_green_data_2 = (temp_green_data_1 << 2) + (temp_green_data_1 >> 4);

				uint32_t comparison_red = 12 * (red_data - temp_red_data_2) / (8 - ((temp_red_data_1 & 0x11) == 0x11 ? 1 : 0));
				uint32_t comparison_blue = 12 * (blue_data - temp_blue_data_2) / (8 - ((temp_blue_data_1 & 0x11) == 0x11 ? 1 : 0));

				uint32_t comparison_green = 12 * (green_data - temp_green_data_2) / (8 - ((temp_green_data_1 & 0x1111) == 0x1111 ? 1 : 0));

				// Handle Red

				uint32_t value_red_1;
				uint32_t value_red_2;

				if (comparison_red < 2)
				{
					value_red_1 = temp_red_data_1;
					value_red_2 = temp_red_data_1;
				}
				else if (comparison_red < 6)
				{
					value_red_1 = temp_red_data_1;
					value_red_2 = temp_red_data_1 + 1;
				}
				else if (comparison_red < 10)
				{
					value_red_1 = temp_red_data_1 + 1;
					value_red_2 = temp_red_data_1;
				}
				else
				{
					value_red_1 = temp_red_data_1 + 1;
					value_red_2 = temp_red_data_1 + 1;
				}

				// Handle Blue

				uint32_t value_blue_1;
				uint32_t value_blue_2;

				if (comparison_blue < 2)
				{
					value_blue_1 = temp_blue_data_1;
					value_blue_2 = temp_blue_data_1;
				}
				else if (comparison_blue < 6)
				{
					value_blue_1 = temp_blue_data_1;
					value_blue_2 = temp_blue_data_1 + 1;
				}
				else if (comparison_blue < 10)
				{
					value_blue_1 = temp_blue_data_1 + 1;
					value_blue_2 = temp_blue_data_1;
				}
				else
				{
					value_blue_1 = temp_blue_data_1 + 1;
					value_blue_2 = temp_blue_data_1 + 1;
				}

				// Handle Green

				uint32_t value_green_1;
				uint32_t value_green_2;

				if (comparison_green < 2)
				{
					value_green_1 = temp_green_data_1;
					value_green_2 = temp_green_data_1;
				}
				else if (comparison_green < 6)
				{
					value_green_1 = temp_green_data_1;
					value_green_2 = temp_green_data_1 + 1;
				}
				else if (comparison_green < 10)
				{
					value_green_1 = temp_green_data_1 + 1;
					value_green_2 = temp_green_data_1;
				}
				else
				{
					value_green_1 = temp_green_data_1 + 1;
					value_green_2 = temp_green_data_1 + 1;
				}

				// Final Colors

				uint32_t value_color_1;
				uint32_t value_color_2;

				value_color_1 = value_red_1 | ((value_green_1 | (value_blue_1 << 6)) << 5);
				value_color_2 = value_red_2 | ((value_green_2 | (value_blue_2 << 6)) << 5);

				uint32_t temp_value_1 = 0;
				uint32_t temp_value_2 = 0;

				if (value_red_1 != value_red_2)
				{
					if (value_red_1 == temp_red_data_1)
					{
						temp_value_1 += comparison_red;
					}
					else
					{
						temp_value_1 += (12 - comparison_red);
					}
					temp_value_2 += 1;
				}

				if (value_blue_1 != value_blue_2)
				{
					if (value_blue_1 == temp_blue_data_1)
					{
						temp_value_1 += comparison_blue;
					}
					else
					{
						temp_value_1 += (12 - comparison_blue);
					}
					temp_value_2 += 1;
				}

				if (value_green_1 != value_green_2)
				{
					if (value_green_1 == temp_green_data_1)
					{
						temp_value_1 += comparison_green;
					}
					else
					{
						temp_value_1 += (12 - comparison_green);
					}
					temp_value_2 += 1;
				}

				if (temp_value_2 > 0)
				{
					temp_value_1 = (temp_value_1 + (temp_value_2 / 2)) / temp_value_2;
				}

				bool special_case_dxt1 = (full_format_data.format.flag_data & FF_DEDUCEDALPHACOMP) && (temp_value_1 == 5 || temp_value_1 == 6 || temp_value_2 != 0);

				if (temp_value_2 > 0 && !special_case_dxt1)
				{
					if (value_color_2 == 0xFFFF)
					{
						temp_value_1 = 12;
						--value_color_1;
					}
					else
					{
						temp_value_1 = 0;
						++value_color_2;
					}
				}

				if (value_color_2 >= value_color_1)
				{
					uint32_t aSwapTemp = value_color_1;
					value_color_1 = value_color_2;
					value_color_2 = aSwapTemp;

					temp_value_1 = 12 - temp_value_1;
				}

				uint32_t color_selected;

				if (special_case_dxt1)
				{
					color_selected = 2;
				}
				else
				{
					if (temp_value_1 < 2)
					{
						color_selected = 0;
					}
					else if (temp_value_1 < 6)
					{
						color_selected = 2;
					}
					else if (temp_value_1 < 10)
					{
						color_selected = 3;
					}
					else
					{
						color_selected = 1;
					}
				}
				// TEMP

				uint64_t temp_value = color_selected | (color_selected << 2) | ((color_selected | (color_selected << 2)) << 4);
				temp_value = temp_value | (temp_value << 8);
				temp_value = temp_value | (temp_value << 16);
				uint64_t final_value = value_color_1 | (value_color_2 << 16) | (temp_value << 32);

				uint32_t pixel_block_position = 0;

				while (pixel_block_position < full_format_data.pixel_blocks)
				{
					// Reading next code
					uint16_t temp_code = 0;
					read_code(huffman_tree_dict, state_data, temp_code);

					need_bits(state_data, 1);
					uint32_t value_data = read_bits(state_data, 1);
					drop_bits(state_data, 1);

					while (temp_code > 0)
					{
						if (!color_bit_map[pixel_block_position])
						{
							if (value_data)
							{
								uint32_t aOffset = full_format_data.bytes_pixel_blocks * (pixel_block_position)+(full_format_data.two_component ? full_format_data.bytes_component : 0);
								memcpy(&(output_data[aOffset]), &final_value, full_format_data.bytes_component);
								color_bit_map[pixel_block_position] = true;
							}
							--temp_code;
						}
						++pixel_block_position;
					}

					while (pixel_block_position < full_format_data.pixel_blocks && color_bit_map[pixel_block_position])
					{
						++pixel_block_position;
					}
				}
			}

			void decode_bptc_float(State& state_data, std::vector<bool>& alpha_bit_map, std::vector<bool>& color_bit_map, const FullFormat& full_format_data, uint8_t* output_data)
			{
				uint32_t pixel_block_position = 0;

				while (pixel_block_position < full_format_data.pixel_blocks)
				{
					// Reading next code
					uint16_t temp_code = 0;
					read_code(huffman_tree_dict, state_data, temp_code);

					need_bits(state_data, 1);
					uint32_t value_data = read_bits(state_data, 1);
					drop_bits(state_data, 1);

					while (temp_code > 0)
					{
						if (!color_bit_map[pixel_block_position])
						{
							if (value_data)
							{
								float max_float_value = 1.0f; // Adjust if necessary
								memcpy(&(output_data[full_format_data.bytes_pixel_blocks * pixel_block_position]), &max_float_value, sizeof(float));

								alpha_bit_map[pixel_block_position] = true;
								color_bit_map[pixel_block_position] = true;
							}
							--temp_code;
						}
						++pixel_block_position;
					}

					while (pixel_block_position < full_format_data.pixel_blocks && color_bit_map[pixel_block_position])
					{
						++pixel_block_position;
					}
				}
			}

			void decode_bptc_unorm(State& state_data, std::vector<bool>& alpha_bit_map, std::vector<bool>& color_bit_map, const FullFormat& full_format_data, uint8_t* output_data)
			{
				uint32_t pixel_block_position = 0;

				while (pixel_block_position < full_format_data.pixel_blocks)
				{
					// Reading next code
					uint16_t temp_code = 0;
					read_code(huffman_tree_dict, state_data, temp_code);

					need_bits(state_data, 1);
					uint32_t value_data = read_bits(state_data, 1);
					drop_bits(state_data, 1);

					while (temp_code > 0)
					{
						if (!color_bit_map[pixel_block_position])
						{
							if (value_data)
							{
								uint8_t max_unorm_value = 255; // Adjust for different bit depths
								memset(&(output_data[full_format_data.bytes_pixel_blocks * pixel_block_position]), max_unorm_value, full_format_data.bytes_component);

								alpha_bit_map[pixel_block_position] = true;
								color_bit_map[pixel_block_position] = true;
							}
							--temp_code;
						}
						++pixel_block_position;
					}

					while (pixel_block_position < full_format_data.pixel_blocks && color_bit_map[pixel_block_position])
					{
						++pixel_block_position;
					}
				}
			}


			void inflate_data(State& state_data, const FullFormat& full_format_data, uint32_t output_data_size, uint8_t* output_data)
			{
				// Bitmaps
				std::vector<bool> color_bitmap_data;
				std::vector<bool> alpha_bitmap_data;

				uint32_t chunk_start_position = state_data.input_position;

				state_data.input_position = chunk_start_position;

				state_data.head = 0;
				state_data.bits = 0;
				state_data.buffer = 0;

				// Getting size of compressed data
				need_bits(state_data, 32);
				uint32_t data_size = read_bits(state_data, 32);
				drop_bits(state_data, 32);

				// Compression Flags
				need_bits(state_data, 32);
				uint32_t compression_flag_data = read_bits(state_data, 32);
				drop_bits(state_data, 32);

				color_bitmap_data.assign(full_format_data.pixel_blocks, false);
				alpha_bitmap_data.assign(full_format_data.pixel_blocks, false);

				if (compression_flag_data & CF_DECODE_WHITE_COLOR)
				{
					decode_white_color(state_data, alpha_bitmap_data, color_bitmap_data, full_format_data, output_data);
				}

				if (compression_flag_data & CF_DECODE_CONSTANT_ALPHA_FROM4BITS)
				{
					decode_constant_alpha_from_4_bits(state_data, alpha_bitmap_data, full_format_data, output_data);
				}

				if (compression_flag_data & CF_DECODE_CONSTANT_ALPHA_FROM8BITS)
				{
					decode_constant_alpha_from_8_bits(state_data, alpha_bitmap_data, full_format_data, output_data);
				}

				if (compression_flag_data & CF_DECODE_PLAIN_COLOR)
				{
					decode_plain_color(state_data, color_bitmap_data, full_format_data, output_data);
				}

				if (compression_flag_data & CF_DECODE_BPTC_FLOAT)
				{
					decode_bptc_float(state_data,alpha_bitmap_data,color_bitmap_data,full_format_data,output_data);
				}

				if (compression_flag_data & CF_DECODE_BPTC_UNORM)
				{
					decode_bptc_unorm(state_data,alpha_bitmap_data,color_bitmap_data,full_format_data,output_data);
				}


				uint32_t loop_index_data;

				if (state_data.bits >= 32)
				{
					--state_data.input_position;
				}

				if ((((full_format_data.format.flag_data) & FF_ALPHA) && !((full_format_data.format.flag_data) & FF_DEDUCEDALPHACOMP)) || (full_format_data.format.flag_data) & FF_BICOLORCOMP)
				{
					for (loop_index_data = 0; loop_index_data < alpha_bitmap_data.size() && state_data.input_position < state_data.input_size; ++loop_index_data)
					{
						if (!alpha_bitmap_data[loop_index_data])
						{
							(*reinterpret_cast<uint32_t*>(&(output_data[full_format_data.bytes_pixel_blocks * loop_index_data]))) = state_data.input[state_data.input_position];
							++state_data.input_position;
							if (full_format_data.bytes_component > 4)
							{
								(*reinterpret_cast<uint32_t*>(&(output_data[full_format_data.bytes_pixel_blocks * loop_index_data + 4]))) = state_data.input[state_data.input_position];
								++state_data.input_position;
							}
						}
					}
				}

				if ((full_format_data.format.flag_data) & FF_COLOR || (full_format_data.format.flag_data) & FF_BICOLORCOMP)
				{
					for (loop_index_data = 0; loop_index_data < color_bitmap_data.size() && state_data.input_position < state_data.input_size; ++loop_index_data)
					{
						if (!color_bitmap_data[loop_index_data])
						{
							uint32_t aOffset = full_format_data.bytes_pixel_blocks * loop_index_data + (full_format_data.two_component ? full_format_data.bytes_component : 0);
							(*reinterpret_cast<uint32_t*>(&(output_data[aOffset]))) = state_data.input[state_data.input_position];
							++state_data.input_position;
						}
					}
					if (full_format_data.bytes_component > 4)
					{
						for (loop_index_data = 0; loop_index_data < color_bitmap_data.size() && state_data.input_position < state_data.input_size; ++loop_index_data)
						{
							if (!color_bitmap_data[loop_index_data])
							{
								uint32_t aOffset = full_format_data.bytes_pixel_blocks * loop_index_data + 4 + (full_format_data.two_component ? full_format_data.bytes_component : 0);
								(*reinterpret_cast<uint32_t*>(&(output_data[aOffset]))) = state_data.input[state_data.input_position];
								++state_data.input_position;
							}
						}
					}
				}
			}
		}

		uint8_t* inflate_texture_file_buffer(uint32_t iinput_size, const uint8_t* input_data, uint32_t& output_data_size, AnetImage& anet_image)
		{
			if (input_data == nullptr)
			{
				throw std::runtime_error("Input buffer is null.");
			}


			uint8_t* temp_output_data(nullptr);
			bool output_data_owned(true);

			try
			{
				if (!texture::static_values_initialized)
				{
					texture::initialize_static_values();
					texture::static_values_initialized = true;
				}

				// Initialize state
				State state_data;
				state_data.input = reinterpret_cast<const uint32_t*>(input_data);
				state_data.input_size = iinput_size / 4;
				state_data.input_position = 0;

				state_data.head = 0;
				state_data.bits = 0;
				state_data.buffer = 0;

				state_data.is_empty = false;

				// Skipping header
				need_bits(state_data, 32);
				anet_image.identifier = read_bits(state_data,32);
				drop_bits(state_data, 32);

				// Format
				need_bits(state_data, 32);
				uint32_t format_four_cc = read_bits(state_data, 32);
				drop_bits(state_data, 32);
				anet_image.format = format_four_cc;
				texture::FullFormat full_format_data;

				full_format_data.format = texture::deduceFormat(format_four_cc);

				// Getting width/height
				need_bits(state_data, 32);
				full_format_data.width = static_cast<uint16_t>(read_bits(state_data, 16));
				drop_bits(state_data, 16);
				full_format_data.height = static_cast<uint16_t>(read_bits(state_data, 16));
				drop_bits(state_data, 16);
				anet_image.width = full_format_data.width;
				anet_image.height = full_format_data.height;

				full_format_data.pixel_blocks = ((full_format_data.width + 3) / 4) * ((full_format_data.height + 3) / 4);
				full_format_data.bytes_pixel_blocks = (full_format_data.format.pixel_size_bits * 4 * 4) / 8;
				full_format_data.two_component =
					((full_format_data.format.flag_data & (texture::FF_PLAINCOMP | texture::FF_COLOR | texture::FF_ALPHA)) == (texture::FF_PLAINCOMP | texture::FF_COLOR | texture::FF_ALPHA)) ||
					(full_format_data.format.flag_data & texture::FF_BICOLORCOMP) ||
					(full_format_data.format.flag_data & texture::FF_BPTC);

				full_format_data.bytes_component = full_format_data.bytes_pixel_blocks / (full_format_data.two_component ? 2 : 1);

				uint32_t output_size = full_format_data.bytes_pixel_blocks * full_format_data.pixel_blocks;

				if (output_data_size != 0 && output_data_size < output_size)
				{
					throw std::runtime_error("Output buffer is too small.");
				}

				output_data_size = output_size;


				temp_output_data = static_cast<uint8_t*>(malloc(sizeof(uint8_t) * output_size));


				texture::inflate_data(state_data, full_format_data, output_data_size, temp_output_data);

				return temp_output_data;
			}
			catch (std::runtime_error& iException)
			{
				if (output_data_owned)
				{
					free(temp_output_data);
				}
				throw iException; // Rethrow exception
			}
			catch (std::exception& iException)
			{
				if (output_data_owned)
				{
					free(temp_output_data);
				}
				throw iException; // Rethrow exception
			}
		}

		uint8_t* inflate_texture_block_buffer(uint16_t iWidth, uint16_t iHeight, uint32_t iFormatFourCc, uint32_t iinput_size, const uint8_t* input_data,
			uint32_t& output_data_size, uint8_t* output_data)
		{
			if (input_data == nullptr)
			{
				throw std::runtime_error("Input buffer is null.");
			}

			if (output_data != nullptr && output_data_size == 0)
			{
				throw std::runtime_error("Output buffer is not null and outputSize is not defined.");
			}

			uint8_t* temp_output_data(nullptr);
			bool output_data_owned(true);

			try
			{
				if (!texture::static_values_initialized)
				{
					texture::initialize_static_values();
					texture::static_values_initialized = true;
				}

				// Initialize format
				texture::FullFormat full_format_data;

				full_format_data.format = texture::deduceFormat(iFormatFourCc);
				full_format_data.width = iWidth;
				full_format_data.height = iHeight;

				full_format_data.pixel_blocks = ((full_format_data.width + 3) / 4) * ((full_format_data.height + 3) / 4);
				full_format_data.bytes_pixel_blocks = (full_format_data.format.pixel_size_bits * 4 * 4) / 8;
				full_format_data.two_component =
					((full_format_data.format.flag_data & (texture::FF_PLAINCOMP | texture::FF_COLOR | texture::FF_ALPHA)) == (texture::FF_PLAINCOMP | texture::FF_COLOR | texture::FF_ALPHA)) ||
					(full_format_data.format.flag_data & texture::FF_BICOLORCOMP) ||
					(full_format_data.format.flag_data & texture::FF_BPTC);

				full_format_data.bytes_component = full_format_data.bytes_pixel_blocks / (full_format_data.two_component ? 2 : 1);

				// Initialize state
				State state_data;
				state_data.input = reinterpret_cast<const uint32_t*>(input_data);
				state_data.input_size = iinput_size / 4;
				state_data.input_position = 0;

				state_data.head = 0;
				state_data.bits = 0;
				state_data.buffer = 0;

				state_data.is_empty = false;

				// Allocate output buffer
				uint32_t output_size = full_format_data.bytes_pixel_blocks * full_format_data.pixel_blocks;

				if (output_data_size != 0 && output_data_size < output_size)
				{
					throw std::runtime_error("Output buffer is too small.");
				}

				output_data_size = output_size;

				if (output_data == nullptr)
				{
					temp_output_data = static_cast<uint8_t*>(malloc(sizeof(uint8_t) * output_size));
				}
				else
				{
					output_data_owned = false;
					temp_output_data = output_data;
				}

				texture::inflate_data(state_data, full_format_data, output_data_size, temp_output_data);

				return temp_output_data;
			}
			catch (std::runtime_error& iException)
			{
				if (output_data_owned)
				{
					free(temp_output_data);
				}
				throw iException; // Rethrow exception
			}
			catch (std::exception& iException)
			{
				if (output_data_owned)
				{
					free(temp_output_data);
				}
				throw iException; // Rethrow exception
			}
		}

	}
}
