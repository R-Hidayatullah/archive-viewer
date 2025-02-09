#ifndef GW2DATTOOLS_COMPRESSION_HUFFMANTREEUTILS_H
#define GW2DATTOOLS_COMPRESSION_HUFFMANTREEUTILS_H

#include <cstdint>
#include <stdexcept>

namespace gw2dt
{
    namespace compression
    {

        static const uint32_t MAX_CODE_BITS_LENGTH = 32; // Max number of bits per code
        static const uint32_t MAX_SYMBOL_VALUE = 285;    // Max value for a symbol
        static const uint32_t MAX_BITS_HASH = 8;

        struct HuffmanTree
        {
            uint32_t code_comparison[MAX_CODE_BITS_LENGTH];
            uint16_t symbol_value_offset[MAX_CODE_BITS_LENGTH];
            uint16_t symbol_value[MAX_SYMBOL_VALUE];
            uint8_t code_bits[MAX_CODE_BITS_LENGTH];

            int16_t symbol_value_hash[1 << MAX_BITS_HASH];
            uint8_t code_bits_hash[1 << MAX_BITS_HASH];

            bool is_empty;
        };

        struct State
        {
            const uint32_t *input;
            uint32_t input_size;
            uint32_t input_position;

            uint32_t head;
            uint32_t buffer;
            uint8_t bits;

            bool is_empty;
        };

        void build_huffmantree(HuffmanTree &huffman_tree_data, int16_t *bits_head_data, int16_t *bits_body_data);
        void fill_working_tab(const uint8_t bits_data, const int16_t symbol_type, int16_t *bits_head_data, int16_t *bits_body_data);

        // Read the next code
        void read_code(const HuffmanTree &huffman_tree_data, State &state_data, uint16_t &code_data);

        // Bits manipulation
        inline void pull_byte(State &state_data)
        {
            // checking that we have less than 32 bits available
            if (state_data.bits >= 32)
            {
                throw std::runtime_error("Tried to pull a value while we still have 32 bits available.");
            }

            // skip the last element of all 65536 bytes blocks
            if ((state_data.input_position + 1) % (0x4000) == 0)
            {
                ++(state_data.input_position);
            }

            // Fetching the next value
            uint32_t value_data = 0;

            // checking that input_position is not out of bounds
            if (state_data.input_position >= state_data.input_size)
            {
                if (state_data.is_empty)
                {
                    throw std::runtime_error("Reached end of input while trying to fetch a new byte.");
                }
                else
                {
                    state_data.is_empty = true;
                }
            }
            else
            {
                value_data = state_data.input[state_data.input_position];
            }

            // Pulling the data into head/buffer given that we need to keep the relevant bits
            if (state_data.bits == 0)
            {
                state_data.head = value_data;
                state_data.buffer = 0;
            }
            else
            {
                state_data.head |= (value_data >> (state_data.bits));
                state_data.buffer = (value_data << (32 - state_data.bits));
            }

            // Updating state variables
            state_data.bits += 32;
            ++(state_data.input_position);
        }

        inline void need_bits(State &state_data, const uint8_t bits_data)
        {
            // checking that we request at most 32 bits
            if (bits_data > 32)
            {
                throw std::runtime_error("Tried to need more than 32 bits.");
            }

            if (state_data.bits < bits_data)
            {
                pull_byte(state_data);
            }
        }

        inline void drop_bits(State &state_data, const uint8_t bits_data)
        {
            // checking that we request at most 32 bits
            if (bits_data > 32)
            {
                throw std::runtime_error("Tried to drop more than 32 bits.");
            }

            if (bits_data > state_data.bits)
            {
                throw std::runtime_error("Tried to drop more bits than we have.");
            }

            // Updating the values to drop the bits
            if (bits_data == 32)
            {
                state_data.head = state_data.buffer;
                state_data.buffer = 0;
            }
            else
            {
                state_data.head <<= bits_data;
                state_data.head |= (state_data.buffer) >> (32 - bits_data);
                state_data.buffer <<= bits_data;
            }

            // update state info
            state_data.bits -= bits_data;
        }

        inline uint32_t read_bits(const State &state_data, const uint8_t bits_data)
        {
            return (state_data.head) >> (32 - bits_data);
        }

    }
}

#endif // GW2DATTOOLS_COMPRESSION_HUFFMANTREEUTILS_H
