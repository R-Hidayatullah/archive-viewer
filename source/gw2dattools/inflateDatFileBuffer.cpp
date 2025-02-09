#include "gw2dattools/inflateDatFileBuffer.h"

#include <cstdlib>
#include <memory.h>
#include <iostream>
#include <stdio.h>

#include "gw2dattools/HuffmanTree.h"
#include "gw2dattools/BitArray.h"

namespace gw2dt
{
	namespace compression
	{
		namespace dat
		{

			const uint32_t MAX_BITS_HASH = 8;
			const uint32_t MAX_CODE_BITS_LENGTH = 32;
			const uint32_t MAX_SYMBOL_VALUE = 285;

			typedef utils::BitArray<uint32_t> DatFileBitArray;
			typedef HuffmanTree<uint16_t, MAX_BITS_HASH, MAX_CODE_BITS_LENGTH, MAX_SYMBOL_VALUE> DatFileHuffmanTree;
			typedef HuffmanTreeBuilder<uint16_t, MAX_CODE_BITS_LENGTH, MAX_SYMBOL_VALUE> DatFileHuffmanTreeBuilder;

			static DatFileHuffmanTree dat_file_huffmantree_dict;

			// Parse and build a huffmanTree
			bool parse_huffmantree(DatFileBitArray& ioInputBitArray, DatFileHuffmanTree& huffman_tree_data, DatFileHuffmanTreeBuilder& ioHuffmanTreeBuilder)
			{
				// Reading the number of symbols to read
				uint16_t symbol_number;
				ioInputBitArray.read(16, symbol_number);
				ioInputBitArray.drop(16);

				if (symbol_number > MAX_SYMBOL_VALUE)
				{
					throw std::runtime_error("Too many symbols to decode.");
				}

				ioHuffmanTreeBuilder.clear();

				int16_t remaining_symbol = symbol_number - 1;

				// Fetching the code repartition
				while (remaining_symbol >= 0)
				{
					uint16_t temp_code;

					dat_file_huffmantree_dict.read_code(ioInputBitArray, temp_code);

					uint8_t temp_code_number_bits = temp_code & 0x1F;
					uint16_t temp_code_number_symbol = (temp_code >> 5) + 1;

					if (temp_code_number_bits == 0)
					{
						remaining_symbol -= temp_code_number_symbol;
					}
					else
					{
						while (temp_code_number_symbol > 0)
						{
							ioHuffmanTreeBuilder.add_symbol(remaining_symbol, temp_code_number_bits);
							--remaining_symbol;
							--temp_code_number_symbol;
						}
					}
				}

				return ioHuffmanTreeBuilder.build_huffmantree(huffman_tree_data);
			}

			void inflatedata(DatFileBitArray& ioInputBitArray, uint32_t output_data_size, uint8_t* output_data)
			{
				uint32_t output_position = 0;
				uint64_t max_count_size = 0;
				// Reading the const write size addition value
				ioInputBitArray.drop(4);

				uint16_t write_size_const_addition;

				ioInputBitArray.read(4, write_size_const_addition);

				write_size_const_addition += 1;

				ioInputBitArray.drop(4);

				// Declaring our HuffmanTrees
				DatFileHuffmanTree huffmantree_symbol;
				DatFileHuffmanTree huffmantree_copy;

				DatFileHuffmanTreeBuilder huffmantree_builder;

				while (output_position < output_data_size)
				{
					// Reading HuffmanTrees
					if (!parse_huffmantree(ioInputBitArray, huffmantree_symbol, huffmantree_builder) || !parse_huffmantree(ioInputBitArray, huffmantree_copy, huffmantree_builder))
					{
						break;
					}

					// Reading MaxCount
					uint32_t max_count;
					ioInputBitArray.read(4, max_count);

					max_count = (max_count + 1) << 12;
					ioInputBitArray.drop(4);
					max_count_size = max_count_size = 1;
					uint32_t current_code_read_count = 0;

					while ((current_code_read_count < max_count) &&
						(output_position < output_data_size))
					{
						++current_code_read_count;

						// Reading next code
						uint16_t symbol_data = 0;
						huffmantree_symbol.read_code(ioInputBitArray, symbol_data);

						if (symbol_data < 0x100)
						{
							output_data[output_position] = static_cast<uint8_t>(symbol_data);
							++output_position;
							continue;
						}

						// We are in copy mode !
						// Reading the additional info to know the write size
						symbol_data -= 0x100;

						// write size
						div_t temp_code_div4 = div(symbol_data, 4);

						uint32_t write_size = 0;
						if (temp_code_div4.quot == 0)
						{
							write_size = symbol_data;
						}
						else if (temp_code_div4.quot < 7)
						{
							write_size = ((1 << (temp_code_div4.quot - 1)) * (4 + temp_code_div4.rem));
						}
						else if (symbol_data == 28)
						{
							write_size = 0xFF;
						}
						else
						{
							throw std::runtime_error("Invalid value for writeSize code.");
						}

						// additional bits
						if (temp_code_div4.quot > 1 && symbol_data != 28)
						{
							uint8_t write_size_add_bits = static_cast<uint8_t>(temp_code_div4.quot - 1);
							uint32_t write_size_add;
							ioInputBitArray.read(write_size_add_bits, write_size_add);
							write_size |= write_size_add;
							ioInputBitArray.drop(write_size_add_bits);
						}
						write_size += write_size_const_addition;

						// write offset
						// Reading the write offset
						huffmantree_copy.read_code(ioInputBitArray, symbol_data);

						div_t temp_code_div2 = div(symbol_data, 2);

						uint32_t write_offset = 0;
						if (temp_code_div2.quot == 0)
						{
							write_offset = symbol_data;
						}
						else if (temp_code_div2.quot < 17)
						{
							write_offset = ((1 << (temp_code_div2.quot - 1)) * (2 + temp_code_div2.rem));
						}
						else
						{
							throw std::runtime_error("Invalid value for writeOffset code.");
						}

						// additional bits
						if (temp_code_div2.quot > 1)
						{
							uint8_t write_offsetAddBits = static_cast<uint8_t>(temp_code_div2.quot - 1);
							uint32_t write_offsetAdd;
							ioInputBitArray.read(write_offsetAddBits, write_offsetAdd);
							write_offset |= write_offsetAdd;
							ioInputBitArray.drop(write_offsetAddBits);
						}
						write_offset += 1;

						uint32_t already_written = 0;
						while ((already_written < write_size) &&
							(output_position < output_data_size))
						{
							output_data[output_position] = output_data[output_position - write_offset];
							++output_position;
							++already_written;
						}
					}
				}
				uint64_t before_eof = 0;
				if ((ioInputBitArray.input_data_size * 8) > ioInputBitArray.position_bits)
				{
					before_eof = (ioInputBitArray.input_data_size * 8) - ioInputBitArray.position_bits;
				}
			}
		}

		uint8_t* inflate_dat_file_buffer(uint32_t input_size, const uint8_t* input_data, uint32_t& output_data_size)
		{
			if (input_data == nullptr)
			{
				throw std::runtime_error("Input buffer is null.");
			}

			uint8_t* temp_output_data(nullptr);
			bool output_data_owned(true);

			try
			{
				dat::DatFileBitArray input_bits_data(input_data, input_size, 16384); // Skipping four bytes every 65k chunk
				// printf("\nBuffer position : %d\n", static_cast<uint32_t>(input_bits_data.buffer_position - input_bits_data.buffer_start_position));
				input_bits_data.input_data_size = input_size;
				// Skipping header & Getting size of the uncompressed data
				input_bits_data.drop(32);

				// Getting size of the uncompressed data
				uint32_t output_size;

				input_bits_data.read(32, output_size);

				input_bits_data.drop(32);

				if (output_data_size != 0)
				{
					output_size = std::min(output_size, output_data_size);
				}


				output_data_size = output_size;


				temp_output_data = static_cast<uint8_t*>(malloc(sizeof(uint8_t) * output_size));


				dat::inflatedata(input_bits_data, output_size, temp_output_data);

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

		class DatFileHuffmanTreeDictStaticInitializer
		{
		public:
			DatFileHuffmanTreeDictStaticInitializer(dat::DatFileHuffmanTree& huffman_tree_data);
		};

		DatFileHuffmanTreeDictStaticInitializer::DatFileHuffmanTreeDictStaticInitializer(dat::DatFileHuffmanTree& huffman_tree_data)
		{
			dat::DatFileHuffmanTreeBuilder dat_file_huffmantree_builder{};
			dat_file_huffmantree_builder.clear();

			dat_file_huffmantree_builder.add_symbol(0x0A, 3);
			dat_file_huffmantree_builder.add_symbol(0x09, 3);
			dat_file_huffmantree_builder.add_symbol(0x08, 3);

			dat_file_huffmantree_builder.add_symbol(0x0C, 4);
			dat_file_huffmantree_builder.add_symbol(0x0B, 4);
			dat_file_huffmantree_builder.add_symbol(0x07, 4);
			dat_file_huffmantree_builder.add_symbol(0x00, 4);

			dat_file_huffmantree_builder.add_symbol(0xE0, 5);
			dat_file_huffmantree_builder.add_symbol(0x2A, 5);
			dat_file_huffmantree_builder.add_symbol(0x29, 5);
			dat_file_huffmantree_builder.add_symbol(0x06, 5);

			dat_file_huffmantree_builder.add_symbol(0x4A, 6);
			dat_file_huffmantree_builder.add_symbol(0x40, 6);
			dat_file_huffmantree_builder.add_symbol(0x2C, 6);
			dat_file_huffmantree_builder.add_symbol(0x2B, 6);
			dat_file_huffmantree_builder.add_symbol(0x28, 6);
			dat_file_huffmantree_builder.add_symbol(0x20, 6);
			dat_file_huffmantree_builder.add_symbol(0x05, 6);
			dat_file_huffmantree_builder.add_symbol(0x04, 6);

			dat_file_huffmantree_builder.add_symbol(0x49, 7);
			dat_file_huffmantree_builder.add_symbol(0x48, 7);
			dat_file_huffmantree_builder.add_symbol(0x27, 7);
			dat_file_huffmantree_builder.add_symbol(0x26, 7);
			dat_file_huffmantree_builder.add_symbol(0x25, 7);
			dat_file_huffmantree_builder.add_symbol(0x0D, 7);
			dat_file_huffmantree_builder.add_symbol(0x03, 7);

			dat_file_huffmantree_builder.add_symbol(0x6A, 8);
			dat_file_huffmantree_builder.add_symbol(0x69, 8);
			dat_file_huffmantree_builder.add_symbol(0x4C, 8);
			dat_file_huffmantree_builder.add_symbol(0x4B, 8);
			dat_file_huffmantree_builder.add_symbol(0x47, 8);
			dat_file_huffmantree_builder.add_symbol(0x24, 8);

			dat_file_huffmantree_builder.add_symbol(0xE8, 9);
			dat_file_huffmantree_builder.add_symbol(0xA0, 9);
			dat_file_huffmantree_builder.add_symbol(0x89, 9);
			dat_file_huffmantree_builder.add_symbol(0x88, 9);
			dat_file_huffmantree_builder.add_symbol(0x68, 9);
			dat_file_huffmantree_builder.add_symbol(0x67, 9);
			dat_file_huffmantree_builder.add_symbol(0x63, 9);
			dat_file_huffmantree_builder.add_symbol(0x60, 9);
			dat_file_huffmantree_builder.add_symbol(0x46, 9);
			dat_file_huffmantree_builder.add_symbol(0x23, 9);

			dat_file_huffmantree_builder.add_symbol(0xE9, 10);
			dat_file_huffmantree_builder.add_symbol(0xC9, 10);
			dat_file_huffmantree_builder.add_symbol(0xC0, 10);
			dat_file_huffmantree_builder.add_symbol(0xA9, 10);
			dat_file_huffmantree_builder.add_symbol(0xA8, 10);
			dat_file_huffmantree_builder.add_symbol(0x8A, 10);
			dat_file_huffmantree_builder.add_symbol(0x87, 10);
			dat_file_huffmantree_builder.add_symbol(0x80, 10);
			dat_file_huffmantree_builder.add_symbol(0x66, 10);
			dat_file_huffmantree_builder.add_symbol(0x65, 10);
			dat_file_huffmantree_builder.add_symbol(0x45, 10);
			dat_file_huffmantree_builder.add_symbol(0x44, 10);
			dat_file_huffmantree_builder.add_symbol(0x43, 10);
			dat_file_huffmantree_builder.add_symbol(0x2D, 10);
			dat_file_huffmantree_builder.add_symbol(0x02, 10);
			dat_file_huffmantree_builder.add_symbol(0x01, 10);

			dat_file_huffmantree_builder.add_symbol(0xE5, 11);
			dat_file_huffmantree_builder.add_symbol(0xC8, 11);
			dat_file_huffmantree_builder.add_symbol(0xAA, 11);
			dat_file_huffmantree_builder.add_symbol(0xA5, 11);
			dat_file_huffmantree_builder.add_symbol(0xA4, 11);
			dat_file_huffmantree_builder.add_symbol(0x8B, 11);
			dat_file_huffmantree_builder.add_symbol(0x85, 11);
			dat_file_huffmantree_builder.add_symbol(0x84, 11);
			dat_file_huffmantree_builder.add_symbol(0x6C, 11);
			dat_file_huffmantree_builder.add_symbol(0x6B, 11);
			dat_file_huffmantree_builder.add_symbol(0x64, 11);
			dat_file_huffmantree_builder.add_symbol(0x4D, 11);
			dat_file_huffmantree_builder.add_symbol(0x0E, 11);

			dat_file_huffmantree_builder.add_symbol(0xE7, 12);
			dat_file_huffmantree_builder.add_symbol(0xCA, 12);
			dat_file_huffmantree_builder.add_symbol(0xC7, 12);
			dat_file_huffmantree_builder.add_symbol(0xA7, 12);
			dat_file_huffmantree_builder.add_symbol(0xA6, 12);
			dat_file_huffmantree_builder.add_symbol(0x86, 12);
			dat_file_huffmantree_builder.add_symbol(0x83, 12);

			dat_file_huffmantree_builder.add_symbol(0xE6, 13);
			dat_file_huffmantree_builder.add_symbol(0xE4, 13);
			dat_file_huffmantree_builder.add_symbol(0xC4, 13);
			dat_file_huffmantree_builder.add_symbol(0x8C, 13);
			dat_file_huffmantree_builder.add_symbol(0x2E, 13);
			dat_file_huffmantree_builder.add_symbol(0x22, 13);

			dat_file_huffmantree_builder.add_symbol(0xEC, 14);
			dat_file_huffmantree_builder.add_symbol(0xC6, 14);
			dat_file_huffmantree_builder.add_symbol(0x6D, 14);
			dat_file_huffmantree_builder.add_symbol(0x4E, 14);

			dat_file_huffmantree_builder.add_symbol(0xEA, 15);
			dat_file_huffmantree_builder.add_symbol(0xCC, 15);
			dat_file_huffmantree_builder.add_symbol(0xAC, 15);
			dat_file_huffmantree_builder.add_symbol(0xAB, 15);
			dat_file_huffmantree_builder.add_symbol(0x8D, 15);
			dat_file_huffmantree_builder.add_symbol(0x11, 15);
			dat_file_huffmantree_builder.add_symbol(0x10, 15);
			dat_file_huffmantree_builder.add_symbol(0x0F, 15);

			dat_file_huffmantree_builder.add_symbol(0xFF, 16);
			dat_file_huffmantree_builder.add_symbol(0xFE, 16);
			dat_file_huffmantree_builder.add_symbol(0xFD, 16);
			dat_file_huffmantree_builder.add_symbol(0xFC, 16);
			dat_file_huffmantree_builder.add_symbol(0xFB, 16);
			dat_file_huffmantree_builder.add_symbol(0xFA, 16);
			dat_file_huffmantree_builder.add_symbol(0xF9, 16);
			dat_file_huffmantree_builder.add_symbol(0xF8, 16);
			dat_file_huffmantree_builder.add_symbol(0xF7, 16);
			dat_file_huffmantree_builder.add_symbol(0xF6, 16);
			dat_file_huffmantree_builder.add_symbol(0xF5, 16);
			dat_file_huffmantree_builder.add_symbol(0xF4, 16);
			dat_file_huffmantree_builder.add_symbol(0xF3, 16);
			dat_file_huffmantree_builder.add_symbol(0xF2, 16);
			dat_file_huffmantree_builder.add_symbol(0xF1, 16);
			dat_file_huffmantree_builder.add_symbol(0xF0, 16);
			dat_file_huffmantree_builder.add_symbol(0xEF, 16);
			dat_file_huffmantree_builder.add_symbol(0xEE, 16);
			dat_file_huffmantree_builder.add_symbol(0xED, 16);
			dat_file_huffmantree_builder.add_symbol(0xEB, 16);
			dat_file_huffmantree_builder.add_symbol(0xE3, 16);
			dat_file_huffmantree_builder.add_symbol(0xE2, 16);
			dat_file_huffmantree_builder.add_symbol(0xE1, 16);
			dat_file_huffmantree_builder.add_symbol(0xDF, 16);
			dat_file_huffmantree_builder.add_symbol(0xDE, 16);
			dat_file_huffmantree_builder.add_symbol(0xDD, 16);
			dat_file_huffmantree_builder.add_symbol(0xDC, 16);
			dat_file_huffmantree_builder.add_symbol(0xDB, 16);
			dat_file_huffmantree_builder.add_symbol(0xDA, 16);
			dat_file_huffmantree_builder.add_symbol(0xD9, 16);
			dat_file_huffmantree_builder.add_symbol(0xD8, 16);
			dat_file_huffmantree_builder.add_symbol(0xD7, 16);
			dat_file_huffmantree_builder.add_symbol(0xD6, 16);
			dat_file_huffmantree_builder.add_symbol(0xD5, 16);
			dat_file_huffmantree_builder.add_symbol(0xD4, 16);
			dat_file_huffmantree_builder.add_symbol(0xD3, 16);
			dat_file_huffmantree_builder.add_symbol(0xD2, 16);
			dat_file_huffmantree_builder.add_symbol(0xD1, 16);
			dat_file_huffmantree_builder.add_symbol(0xD0, 16);
			dat_file_huffmantree_builder.add_symbol(0xCF, 16);
			dat_file_huffmantree_builder.add_symbol(0xCE, 16);
			dat_file_huffmantree_builder.add_symbol(0xCD, 16);
			dat_file_huffmantree_builder.add_symbol(0xCB, 16);
			dat_file_huffmantree_builder.add_symbol(0xC5, 16);
			dat_file_huffmantree_builder.add_symbol(0xC3, 16);
			dat_file_huffmantree_builder.add_symbol(0xC2, 16);
			dat_file_huffmantree_builder.add_symbol(0xC1, 16);
			dat_file_huffmantree_builder.add_symbol(0xBF, 16);
			dat_file_huffmantree_builder.add_symbol(0xBE, 16);
			dat_file_huffmantree_builder.add_symbol(0xBD, 16);
			dat_file_huffmantree_builder.add_symbol(0xBC, 16);
			dat_file_huffmantree_builder.add_symbol(0xBB, 16);
			dat_file_huffmantree_builder.add_symbol(0xBA, 16);
			dat_file_huffmantree_builder.add_symbol(0xB9, 16);
			dat_file_huffmantree_builder.add_symbol(0xB8, 16);
			dat_file_huffmantree_builder.add_symbol(0xB7, 16);
			dat_file_huffmantree_builder.add_symbol(0xB6, 16);
			dat_file_huffmantree_builder.add_symbol(0xB5, 16);
			dat_file_huffmantree_builder.add_symbol(0xB4, 16);
			dat_file_huffmantree_builder.add_symbol(0xB3, 16);
			dat_file_huffmantree_builder.add_symbol(0xB2, 16);
			dat_file_huffmantree_builder.add_symbol(0xB1, 16);
			dat_file_huffmantree_builder.add_symbol(0xB0, 16);
			dat_file_huffmantree_builder.add_symbol(0xAF, 16);
			dat_file_huffmantree_builder.add_symbol(0xAE, 16);
			dat_file_huffmantree_builder.add_symbol(0xAD, 16);
			dat_file_huffmantree_builder.add_symbol(0xA3, 16);
			dat_file_huffmantree_builder.add_symbol(0xA2, 16);
			dat_file_huffmantree_builder.add_symbol(0xA1, 16);
			dat_file_huffmantree_builder.add_symbol(0x9F, 16);
			dat_file_huffmantree_builder.add_symbol(0x9E, 16);
			dat_file_huffmantree_builder.add_symbol(0x9D, 16);
			dat_file_huffmantree_builder.add_symbol(0x9C, 16);
			dat_file_huffmantree_builder.add_symbol(0x9B, 16);
			dat_file_huffmantree_builder.add_symbol(0x9A, 16);
			dat_file_huffmantree_builder.add_symbol(0x99, 16);
			dat_file_huffmantree_builder.add_symbol(0x98, 16);
			dat_file_huffmantree_builder.add_symbol(0x97, 16);
			dat_file_huffmantree_builder.add_symbol(0x96, 16);
			dat_file_huffmantree_builder.add_symbol(0x95, 16);
			dat_file_huffmantree_builder.add_symbol(0x94, 16);
			dat_file_huffmantree_builder.add_symbol(0x93, 16);
			dat_file_huffmantree_builder.add_symbol(0x92, 16);
			dat_file_huffmantree_builder.add_symbol(0x91, 16);
			dat_file_huffmantree_builder.add_symbol(0x90, 16);
			dat_file_huffmantree_builder.add_symbol(0x8F, 16);
			dat_file_huffmantree_builder.add_symbol(0x8E, 16);
			dat_file_huffmantree_builder.add_symbol(0x82, 16);
			dat_file_huffmantree_builder.add_symbol(0x81, 16);
			dat_file_huffmantree_builder.add_symbol(0x7F, 16);
			dat_file_huffmantree_builder.add_symbol(0x7E, 16);
			dat_file_huffmantree_builder.add_symbol(0x7D, 16);
			dat_file_huffmantree_builder.add_symbol(0x7C, 16);
			dat_file_huffmantree_builder.add_symbol(0x7B, 16);
			dat_file_huffmantree_builder.add_symbol(0x7A, 16);
			dat_file_huffmantree_builder.add_symbol(0x79, 16);
			dat_file_huffmantree_builder.add_symbol(0x78, 16);
			dat_file_huffmantree_builder.add_symbol(0x77, 16);
			dat_file_huffmantree_builder.add_symbol(0x76, 16);
			dat_file_huffmantree_builder.add_symbol(0x75, 16);
			dat_file_huffmantree_builder.add_symbol(0x74, 16);
			dat_file_huffmantree_builder.add_symbol(0x73, 16);
			dat_file_huffmantree_builder.add_symbol(0x72, 16);
			dat_file_huffmantree_builder.add_symbol(0x71, 16);
			dat_file_huffmantree_builder.add_symbol(0x70, 16);
			dat_file_huffmantree_builder.add_symbol(0x6F, 16);
			dat_file_huffmantree_builder.add_symbol(0x6E, 16);
			dat_file_huffmantree_builder.add_symbol(0x62, 16);
			dat_file_huffmantree_builder.add_symbol(0x61, 16);
			dat_file_huffmantree_builder.add_symbol(0x5F, 16);
			dat_file_huffmantree_builder.add_symbol(0x5E, 16);
			dat_file_huffmantree_builder.add_symbol(0x5D, 16);
			dat_file_huffmantree_builder.add_symbol(0x5C, 16);
			dat_file_huffmantree_builder.add_symbol(0x5B, 16);
			dat_file_huffmantree_builder.add_symbol(0x5A, 16);
			dat_file_huffmantree_builder.add_symbol(0x59, 16);
			dat_file_huffmantree_builder.add_symbol(0x58, 16);
			dat_file_huffmantree_builder.add_symbol(0x57, 16);
			dat_file_huffmantree_builder.add_symbol(0x56, 16);
			dat_file_huffmantree_builder.add_symbol(0x55, 16);
			dat_file_huffmantree_builder.add_symbol(0x54, 16);
			dat_file_huffmantree_builder.add_symbol(0x53, 16);
			dat_file_huffmantree_builder.add_symbol(0x52, 16);
			dat_file_huffmantree_builder.add_symbol(0x51, 16);
			dat_file_huffmantree_builder.add_symbol(0x50, 16);
			dat_file_huffmantree_builder.add_symbol(0x4F, 16);
			dat_file_huffmantree_builder.add_symbol(0x42, 16);
			dat_file_huffmantree_builder.add_symbol(0x41, 16);
			dat_file_huffmantree_builder.add_symbol(0x3F, 16);
			dat_file_huffmantree_builder.add_symbol(0x3E, 16);
			dat_file_huffmantree_builder.add_symbol(0x3D, 16);
			dat_file_huffmantree_builder.add_symbol(0x3C, 16);
			dat_file_huffmantree_builder.add_symbol(0x3B, 16);
			dat_file_huffmantree_builder.add_symbol(0x3A, 16);
			dat_file_huffmantree_builder.add_symbol(0x39, 16);
			dat_file_huffmantree_builder.add_symbol(0x38, 16);
			dat_file_huffmantree_builder.add_symbol(0x37, 16);
			dat_file_huffmantree_builder.add_symbol(0x36, 16);
			dat_file_huffmantree_builder.add_symbol(0x35, 16);
			dat_file_huffmantree_builder.add_symbol(0x34, 16);
			dat_file_huffmantree_builder.add_symbol(0x33, 16);
			dat_file_huffmantree_builder.add_symbol(0x32, 16);
			dat_file_huffmantree_builder.add_symbol(0x31, 16);
			dat_file_huffmantree_builder.add_symbol(0x30, 16);
			dat_file_huffmantree_builder.add_symbol(0x2F, 16);
			dat_file_huffmantree_builder.add_symbol(0x21, 16);
			dat_file_huffmantree_builder.add_symbol(0x1F, 16);
			dat_file_huffmantree_builder.add_symbol(0x1E, 16);
			dat_file_huffmantree_builder.add_symbol(0x1D, 16);
			dat_file_huffmantree_builder.add_symbol(0x1C, 16);
			dat_file_huffmantree_builder.add_symbol(0x1B, 16);
			dat_file_huffmantree_builder.add_symbol(0x1A, 16);
			dat_file_huffmantree_builder.add_symbol(0x19, 16);
			dat_file_huffmantree_builder.add_symbol(0x18, 16);
			dat_file_huffmantree_builder.add_symbol(0x17, 16);
			dat_file_huffmantree_builder.add_symbol(0x16, 16);
			dat_file_huffmantree_builder.add_symbol(0x15, 16);
			dat_file_huffmantree_builder.add_symbol(0x14, 16);
			dat_file_huffmantree_builder.add_symbol(0x13, 16);
			dat_file_huffmantree_builder.add_symbol(0x12, 16);

			dat_file_huffmantree_builder.build_huffmantree(huffman_tree_data);
		}

		static DatFileHuffmanTreeDictStaticInitializer dat_file_huffmantree_dict_static_initializer(dat::dat_file_huffmantree_dict);

	}
}
