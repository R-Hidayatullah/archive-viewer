#ifndef GW2DATTOOLS_COMPRESSION_HUFFMANTREE_H
#define GW2DATTOOLS_COMPRESSION_HUFFMANTREE_H

#include <array>
#include <cstdint>
#include <stdexcept>

#include "gw2dattools/BitArray.h"

namespace gw2dt
{
    namespace compression
    {

        template <typename SymbolType,
                  uint8_t MAX_CODE_BITS_LENGTH,
                  uint16_t MAX_SYMBOL_VALUE>
        class HuffmanTreeBuilder;

        // Assumption: code length <= 32
        template <typename SymbolType,
                  uint8_t MAX_BITS_HASH,
                  uint8_t MAX_CODE_BITS_LENGTH,
                  uint16_t MAX_SYMBOL_VALUE>
        class HuffmanTree
        {
        public:
            friend class HuffmanTreeBuilder<SymbolType, MAX_CODE_BITS_LENGTH, MAX_SYMBOL_VALUE>;

            template <typename IntType>
            void read_code(utils::BitArray<IntType> &ioInputBitArray, SymbolType &symbol_data) const;

        private:
            void clear();

            std::array<uint32_t, MAX_CODE_BITS_LENGTH> code_comparison;
            std::array<uint16_t, MAX_CODE_BITS_LENGTH> symbol_value_offset;
            std::array<SymbolType, MAX_SYMBOL_VALUE> symbol_value;
            std::array<uint8_t, MAX_CODE_BITS_LENGTH> code_bits;

            std::array<bool, (1 << MAX_BITS_HASH)> symbol_value_hash_exist;
            std::array<SymbolType, (1 << MAX_BITS_HASH)> symbol_value_hash;
            std::array<uint8_t, (1 << MAX_BITS_HASH)> code_bits_hash;
        };

        template <typename SymbolType,
                  uint8_t MAX_CODE_BITS_LENGTH,
                  uint16_t MAX_SYMBOL_VALUE>
        class HuffmanTreeBuilder
        {
        public:
            void clear();

            void add_symbol(SymbolType symbol_type, uint8_t bits_data);

            template <uint8_t MAX_BITS_HASH>
            bool build_huffmantree(HuffmanTree<SymbolType, MAX_BITS_HASH, MAX_CODE_BITS_LENGTH, MAX_SYMBOL_VALUE> &oHuffmanTree);

        private:
            bool empty() const;

            std::array<bool, MAX_CODE_BITS_LENGTH> bits_head_exist;
            std::array<SymbolType, MAX_CODE_BITS_LENGTH> bits_head;

            std::array<bool, MAX_SYMBOL_VALUE> bits_body_exist;
            std::array<SymbolType, MAX_SYMBOL_VALUE> bits_body;
        };

    }
}

#include "gw2dattools/HuffmanTree.i"

#endif // GW2DATTOOLS_COMPRESSION_HUFFMANTREE_H
