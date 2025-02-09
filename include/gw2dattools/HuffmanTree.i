#ifndef GW2DATTOOLS_COMPRESSION_HUFFMANTREE_I
#define GW2DATTOOLS_COMPRESSION_HUFFMANTREE_I

#include "gw2dattools/BitArray.h"

namespace gw2dt
{
    namespace compression
    {

        template <typename SymbolType,
                  uint8_t MAX_BITS_HASH,
                  uint8_t MAX_CODE_BITS_LENGTH,
                  uint16_t MAX_SYMBOL_VALUE>
        void HuffmanTree<SymbolType, MAX_BITS_HASH, MAX_CODE_BITS_LENGTH, MAX_SYMBOL_VALUE>::clear()
        {
            code_comparison.fill(0);
            symbol_value_offset.fill(0);
            symbol_value.fill(0);
            code_bits.fill(0);

            symbol_value_hash_exist.fill(false);
            symbol_value_hash.fill(0);
            code_bits_hash.fill(0);
        }

        template <typename SymbolType,
                  uint8_t MAX_BITS_HASH,
                  uint8_t MAX_CODE_BITS_LENGTH,
                  uint16_t MAX_SYMBOL_VALUE>
        template <typename IntType>
        void HuffmanTree<SymbolType, MAX_BITS_HASH, MAX_CODE_BITS_LENGTH, MAX_SYMBOL_VALUE>::read_code(utils::BitArray<IntType> &ioInputBitArray, SymbolType &symbol_data) const
        {
            uint32_t hash_value;
            ioInputBitArray.read_lazy(MAX_BITS_HASH, hash_value);

            bool exist = symbol_value_hash_exist[hash_value];

            if (exist)
            {
                symbol_data = symbol_value_hash[hash_value];
                IntType code_bits_hash_data = code_bits_hash[hash_value];

                ioInputBitArray.drop(code_bits_hash_data);
            }
            else
            {
                ioInputBitArray.read_lazy(hash_value);

                uint16_t index_data = 0;
                while (hash_value < code_comparison[index_data])
                {
                    ++index_data;
                }

                uint8_t temp_bits = code_bits[index_data];
                symbol_data = symbol_value[symbol_value_offset[index_data] -
                                           ((hash_value - code_comparison[index_data]) >> (32 - temp_bits))];

                ioInputBitArray.drop(temp_bits);
            }
        }

        template <typename SymbolType,
                  uint8_t MAX_CODE_BITS_LENGTH,
                  uint16_t MAX_SYMBOL_VALUE>
        void HuffmanTreeBuilder<SymbolType, MAX_CODE_BITS_LENGTH, MAX_SYMBOL_VALUE>::clear()
        {
            bits_head_exist.fill(false);
            bits_head.fill(0);

            bits_body_exist.fill(false);
            bits_body.fill(0);
        }

        template <typename SymbolType,
                  uint8_t MAX_CODE_BITS_LENGTH,
                  uint16_t MAX_SYMBOL_VALUE>
        void HuffmanTreeBuilder<SymbolType, MAX_CODE_BITS_LENGTH, MAX_SYMBOL_VALUE>::add_symbol(SymbolType symbol_type, uint8_t bits_data)
        {

            if (bits_head_exist[bits_data])
            {
                bits_body[symbol_type] = bits_head[bits_data];
                bits_body_exist[symbol_type] = true;
                bits_head[bits_data] = symbol_type;
            }
            else
            {
                bits_head[bits_data] = symbol_type;
                bits_head_exist[bits_data] = true;
            }
        }

        template <typename SymbolType,
                  uint8_t MAX_CODE_BITS_LENGTH,
                  uint16_t MAX_SYMBOL_VALUE>
        bool HuffmanTreeBuilder<SymbolType, MAX_CODE_BITS_LENGTH, MAX_SYMBOL_VALUE>::empty() const
        {
            for (auto &it : bits_head_exist)
            {
                if (it == true)
                {
                    return false;
                }
            }
            return true;
        }

        template <typename SymbolType,
                  uint8_t MAX_CODE_BITS_LENGTH,
                  uint16_t MAX_SYMBOL_VALUE>
        template <uint8_t MAX_BITS_HASH>
        bool HuffmanTreeBuilder<SymbolType, MAX_CODE_BITS_LENGTH, MAX_SYMBOL_VALUE>::build_huffmantree(HuffmanTree<SymbolType, MAX_BITS_HASH, MAX_CODE_BITS_LENGTH, MAX_SYMBOL_VALUE> &oHuffmanTree)
        {
            if (empty())
            {
                return false;
            }

            oHuffmanTree.clear();

            // Building the HuffmanTree
            uint32_t temp_code = 0;
            uint8_t temp_bits = 0;

            // First part, filling hashTable for codes that are of less than 8 bits
            while (temp_bits <= MAX_BITS_HASH)
            {
                bool data_exist = bits_head_exist[temp_bits];

                if (data_exist)
                {
                    SymbolType current_symbol = bits_head[temp_bits];

                    while (data_exist)
                    {
                        // Processing hash values
                        uint16_t hash_value = static_cast<uint16_t>(temp_code << (MAX_BITS_HASH - temp_bits));
                        uint16_t next_hash_value = static_cast<uint16_t>((temp_code + 1) << (MAX_BITS_HASH - temp_bits));

                        while (hash_value < next_hash_value)
                        {
                            oHuffmanTree.symbol_value_hash_exist[hash_value] = true;
                            oHuffmanTree.symbol_value_hash[hash_value] = current_symbol;
                            oHuffmanTree.code_bits_hash[hash_value] = temp_bits;
                            ++hash_value;
                        }

                        data_exist = bits_body_exist[current_symbol];
                        current_symbol = bits_body[current_symbol];
                        --temp_code;
                    }
                }

                temp_code = (temp_code << 1) + 1;
                ++temp_bits;
            }

            uint16_t temp_code_comparison_index = 0;
            uint16_t symbol_offset = 0;

            // Second part, filling classical structure for other codes
            while (temp_bits < MAX_CODE_BITS_LENGTH)
            {
                bool data_exist = bits_head_exist[temp_bits];

                if (data_exist)
                {
                    SymbolType current_symbol = bits_head[temp_bits];

                    while (data_exist)
                    {
                        // Registering the code
                        oHuffmanTree.symbol_value[symbol_offset] = current_symbol;

                        ++symbol_offset;
                        data_exist = bits_body_exist[current_symbol];
                        current_symbol = bits_body[current_symbol];

                        --temp_code;
                    }

                    // Minimum code value for temp_bits bits
                    oHuffmanTree.code_comparison[temp_code_comparison_index] = ((temp_code + 1) << (32 - temp_bits));

                    // Number of bits for l_codeCompIndex index
                    oHuffmanTree.code_bits[temp_code_comparison_index] = temp_bits;

                    // Offset in symbol_value table to reach the value
                    oHuffmanTree.symbol_value_offset[temp_code_comparison_index] = symbol_offset - 1;

                    ++temp_code_comparison_index;
                }

                temp_code = (temp_code << 1) + 1;
                ++temp_bits;
            }

            return true;
        }

    }
}

#endif // GW2DATTOOLS_COMPRESSION_HUFFMANTREE_I
