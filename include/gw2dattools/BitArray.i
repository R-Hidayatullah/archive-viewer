#ifndef GW2DATTOOLS_UTILS_BITARRAY_I
#define GW2DATTOOLS_UTILS_BITARRAY_I

#include <cassert>

namespace gw2dt
{
    namespace utils
    {

        template <typename IntType>
        BitArray<IntType>::BitArray(const uint8_t *ipBuffer, uint32_t iSize, uint32_t iSkippedBytes) : buffer_start_position(ipBuffer),
                                                                                                       buffer_position(ipBuffer),
                                                                                                       bytes_available(iSize),
                                                                                                       skipped_bytes(iSkippedBytes),
                                                                                                       head_data(0),
                                                                                                       buffer_data(0),
                                                                                                       bytes_available_data(0)
        {
            assert(iSize % sizeof(uint32_t) == 0);

            pull(head_data, bytes_available_data);
        }

        template <typename IntType>
        void BitArray<IntType>::pull(uint32_t &head_data, uint8_t &bytes_available_data)
        {
            if (bytes_available >= sizeof(uint32_t))
            {
                head_data = *(reinterpret_cast<const uint32_t *>(buffer_position));
                position_bits=position_bits+32;
                bytes_available -= sizeof(uint32_t);
                buffer_position += sizeof(uint32_t);
                bytes_available_data = sizeof(uint32_t) * 8;
            }
            else
            {
                head_data = 0;
                bytes_available_data = 0;
            }
        }

        template <typename IntType>
        template <typename OutputType>
        void BitArray<IntType>::read_impl(uint8_t bits_number, OutputType &value_data) const
        {
            value_data = (head_data >> ((sizeof(uint32_t) * 8) - bits_number));
        }

        template <typename IntType>
        template <typename OutputType>
        void BitArray<IntType>::read_lazy(uint8_t bits_number, OutputType &value_data) const
        {
            if (bits_number > sizeof(OutputType) * 8)
            {
                throw std::runtime_error("Invalid number of bits requested.");
            }
            if (bits_number > sizeof(uint32_t) * 8)
            {
                throw std::runtime_error("Invalid number of bits requested.");
            }

            read_impl(bits_number, value_data);
        }

        template <typename IntType>
        template <uint8_t bits_number, typename OutputType>
        void BitArray<IntType>::read_lazy(OutputType &value_data) const
        {
            static_assert(bits_number <= sizeof(OutputType) * 8, "bits_number must be inferior to the size of the requested type.");
            static_assert(bits_number <= sizeof(uint32_t) * 8, "bits_number must be inferior to the size of the internal type.");

            read_impl(bits_number, value_data);
        }

        template <typename IntType>
        template <typename OutputType>
        void BitArray<IntType>::read_lazy(OutputType &value_data) const
        {
            read_lazy<sizeof(OutputType) * 8>(value_data);
        }

        template <typename IntType>
        template <typename OutputType>
        void BitArray<IntType>::read(uint8_t bits_number, OutputType &value_data) const
        {
            if (bytes_available_data < bits_number)
            {
                throw std::runtime_error("Not enough bits available to read the value.");
            }
            read_lazy(bits_number, value_data);
        }

        template <typename IntType>
        void BitArray<IntType>::drop_impl(uint8_t bits_number)
        {
            if (bytes_available_data < bits_number)
            {
                throw std::runtime_error("Too much bits were asked to be dropped.");
            }

            uint8_t new_bits_available = bytes_available_data - bits_number;
            position_bits=position_bits+bits_number;
            if (new_bits_available >= sizeof(uint32_t) * 8)
            {
                if (bits_number == sizeof(uint32_t) * 8)
                {
                    head_data = buffer_data;
                    buffer_data = 0;
                }
                else
                {
                    head_data = (head_data << bits_number) | (buffer_data >> ((sizeof(uint32_t) * 8) - bits_number));
                    buffer_data = buffer_data << bits_number;
                }
                bytes_available_data = new_bits_available;
            }
            else
            {
                IntType new_value;
                uint8_t pulled_bits;
                pull(new_value, pulled_bits);

                if (bits_number == sizeof(uint32_t) * 8)
                {
                    head_data = 0;
                }
                else
                {
                    head_data = head_data << bits_number;
                }
                head_data |= (buffer_data >> ((sizeof(uint32_t) * 8) - bits_number)) | (new_value >> (new_bits_available));
                if (new_bits_available > 0)
                {
                    buffer_data = new_value << ((sizeof(uint32_t) * 8) - new_bits_available);
                }
                bytes_available_data = new_bits_available + pulled_bits;
            }
        }

        template <typename IntType>
        void BitArray<IntType>::drop(uint8_t bits_number)
        {
            if (bits_number > sizeof(uint32_t) * 8)
            {
                throw std::runtime_error("Invalid number of bits to be dropped.");
            }
            drop_impl(bits_number);
        }

    }
}

#endif // GW2DATTOOLS_UTILS_BITARRAY_I
