#ifndef GW2DATTOOLS_UTILS_BITARRAY_H
#define GW2DATTOOLS_UTILS_BITARRAY_H

#include <cstdint>
#include <stdexcept>

namespace gw2dt
{
    namespace utils
    {

        template <typename IntType>
        class BitArray
        {
        public:
            BitArray(const uint8_t *ipBuffer, uint32_t iSize, uint32_t iSkippedBytes = 0);

            template <typename OutputType>
            void read_lazy(uint8_t bits_number, OutputType &value_data) const;

            template <uint8_t bits_number, typename OutputType>
            void read_lazy(OutputType &value_data) const;

            template <typename OutputType>
            void read_lazy(OutputType &value_data) const;

            template <typename OutputType>
            void read(uint8_t bits_number, OutputType &value_data) const;
            void drop(uint8_t bits_number);

            const uint8_t *const buffer_start_position;
            const uint8_t *buffer_position;
            uint64_t position_bits;
            uint64_t input_data_size;
        private:
            template <typename OutputType>
            void read_impl(uint8_t bits_number, OutputType &value_data) const;
            void drop_impl(uint8_t bits_number);
            void pull(uint32_t &value_data, uint8_t &oNbPulledBits);

            uint32_t bytes_available;
            uint32_t skipped_bytes;
            uint32_t head_data;
            uint32_t buffer_data;
            uint8_t bytes_available_data;
        };

    }
}

#include "gw2dattools/BitArray.i"

#endif // GW2DATTOOLS_UTILS_BITARRAY_H
