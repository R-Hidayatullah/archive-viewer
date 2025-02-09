#include "gw2dattools/huffmanTreeUtils.h"

#include <memory.h>

namespace gw2dt
{
    namespace compression
    {

        void read_code(const HuffmanTree &huffman_tree_data, State &state_data, uint16_t &code_data)
        {
            if (huffman_tree_data.is_empty)
            {
                throw std::runtime_error("Trying to read code from an empty HuffmanTree.");
            }

            need_bits(state_data, 32);

            if (huffman_tree_data.symbol_value_hash[read_bits(state_data, MAX_BITS_HASH)] != -1)
            {
                code_data = huffman_tree_data.symbol_value_hash[read_bits(state_data, MAX_BITS_HASH)];
                drop_bits(state_data, huffman_tree_data.code_bits_hash[read_bits(state_data, MAX_BITS_HASH)]);
            }
            else
            {
                uint16_t index_data = 0;
                while (read_bits(state_data, 32) < huffman_tree_data.code_comparison[index_data])
                {
                    ++index_data;
                }

                uint8_t temp_bits = huffman_tree_data.code_bits[index_data];
                code_data = huffman_tree_data.symbol_value[huffman_tree_data.symbol_value_offset[index_data] -
                                                           ((read_bits(state_data, 32) - huffman_tree_data.code_comparison[index_data]) >> (32 - temp_bits))];
                drop_bits(state_data, temp_bits);
            }
        }

        void build_huffmantree(HuffmanTree &huffman_tree_data, int16_t *bits_head_data, int16_t *bits_body_data)
        {
            // Resetting Huffmantrees
            memset(&huffman_tree_data.code_comparison, 0, sizeof(huffman_tree_data.code_comparison));
            memset(&huffman_tree_data.symbol_value_offset, 0, sizeof(huffman_tree_data.symbol_value_offset));
            memset(&huffman_tree_data.symbol_value, 0, sizeof(huffman_tree_data.symbol_value));
            memset(&huffman_tree_data.code_bits, 0, sizeof(huffman_tree_data.code_bits));
            memset(&huffman_tree_data.code_bits_hash, 0, sizeof(huffman_tree_data.code_bits_hash));

            memset(&huffman_tree_data.symbol_value_hash, 0xFF, sizeof(huffman_tree_data.symbol_value_hash));

            huffman_tree_data.is_empty = true;

            // Building the HuffmanTree
            uint32_t temp_code = 0;
            uint8_t temp_bits = 0;

            // First part, filling hashTable for codes that are of less than 8 bits
            while (temp_bits <= MAX_BITS_HASH)
            {
                if (bits_head_data[temp_bits] != -1)
                {
                    huffman_tree_data.is_empty = false;

                    int16_t current_symbol = bits_head_data[temp_bits];
                    while (current_symbol != -1)
                    {
                        // Processing hash values
                        uint16_t hash_value = static_cast<uint16_t>(temp_code << (MAX_BITS_HASH - temp_bits));
                        uint16_t next_hash_value = static_cast<uint16_t>((temp_code + 1) << (MAX_BITS_HASH - temp_bits));

                        while (hash_value < next_hash_value)
                        {
                            huffman_tree_data.symbol_value_hash[hash_value] = current_symbol;
                            huffman_tree_data.code_bits_hash[hash_value] = temp_bits;
                            ++hash_value;
                        }

                        current_symbol = bits_body_data[current_symbol];
                        --temp_code;
                    }
                }
                temp_code = (temp_code << 1) + 1;
                ++temp_bits;
            }

            uint16_t code_comparison_index = 0;
            uint16_t symbol_offset = 0;

            // Second part, filling classical structure for other codes
            while (temp_bits < MAX_CODE_BITS_LENGTH)
            {
                if (bits_head_data[temp_bits] != -1)
                {
                    huffman_tree_data.is_empty = false;

                    int16_t current_symbol = bits_head_data[temp_bits];
                    while (current_symbol != -1)
                    {
                        // Registering the code
                        huffman_tree_data.symbol_value[symbol_offset] = current_symbol;

                        ++symbol_offset;
                        current_symbol = bits_body_data[current_symbol];
                        --temp_code;
                    }

                    // Minimum code value for temp_bits bits
                    huffman_tree_data.code_comparison[code_comparison_index] = ((temp_code + 1) << (32 - temp_bits));

                    // Number of bits for l_codeCompIndex index
                    huffman_tree_data.code_bits[code_comparison_index] = temp_bits;

                    // Offset in symbol_value table to reach the value
                    huffman_tree_data.symbol_value_offset[code_comparison_index] = symbol_offset - 1;

                    ++code_comparison_index;
                }
                temp_code = (temp_code << 1) + 1;
                ++temp_bits;
            }
        }

        void fill_working_tab(const uint8_t bits_data, const int16_t symbol_type, int16_t *bits_head_data, int16_t *bits_body_data)
        {
            // checking out of bounds
            if (bits_data >= MAX_CODE_BITS_LENGTH)
            {
                throw std::runtime_error("Too many bits.");
            }

            if (symbol_type >= static_cast<uint16_t>(MAX_SYMBOL_VALUE))
            {
                throw std::runtime_error("Too high symbol.");
            }

            if (bits_head_data[bits_data] == -1)
            {
                bits_head_data[bits_data] = symbol_type;
            }
            else
            {
                bits_body_data[symbol_type] = bits_head_data[bits_data];
                bits_head_data[bits_data] = symbol_type;
            }
        }

    }
}
