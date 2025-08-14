#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <stdexcept>
#include <iomanip>
#include <algorithm>
#include <unordered_map>
#include <cstdint>
#include <cstring>


#include "gw2dattools/inflateDatFileBuffer.h"
#include "gw2dattools/inflateTextureFileBuffer.h"

constexpr size_t MFT_MAGIC_NUMBER = 4;
constexpr size_t ANET_IMAGE_MAGIC_NUMBER = 4;
constexpr size_t DAT_MAGIC_NUMBER = 3;
constexpr size_t MFT_ENTRY_INDEX_NUM = 1;
constexpr size_t CHUNK_SIZE = 0x10000;
constexpr size_t START_INDEX = CHUNK_SIZE - 4;
constexpr size_t END_INDEX = CHUNK_SIZE;

struct DatHeader
{
	uint8_t version;
	char identifier[DAT_MAGIC_NUMBER];
	uint32_t header_size;
	uint32_t unknown_field;
	uint32_t chunk_size;
	uint32_t crc;
	uint32_t unknown_field_2;
	uint64_t mft_offset;
	uint32_t mft_size;
	uint32_t flag;
};

struct MftData
{
	uint64_t original_index;
	uint64_t offset;
	uint32_t size;
	uint16_t compression_flag;
	uint16_t entry_flag;
	uint32_t counter;
	uint32_t crc;
	uint32_t uncompressed_size;
};

struct MftHeader
{
	char identifier[MFT_MAGIC_NUMBER];
	uint64_t unknown_field;
	uint32_t mft_entry_size;
	uint32_t unknown_field_2;
	uint32_t unknown_field_3;
};

struct MftFileIdData
{
	uint32_t file_id;
	uint32_t base_id;
};

struct MftBaseIdData
{
	std::vector<uint32_t> file_id;
	uint32_t base_id;
};

struct FileInfo {
	std::string file_path;
	uint64_t file_size;
};

struct Gw2Dat
{
	std::ifstream dat_file;
	FileInfo file_info;
	DatHeader dat_header;
	MftHeader mft_header;
	std::vector<MftData> mft_data_list;
	std::vector<MftFileIdData> mft_file_id_data_list;
	std::vector<MftBaseIdData> mft_base_id_data_list;

};

template <typename T>
void read_from_file(std::ifstream& file, T& value) {
	if (!file.read(reinterpret_cast<char*>(&value), sizeof(T))) {
		throw std::runtime_error("Failed to read binary data from file.");
	}
}

template <typename T, size_t N>
void read_from_file(std::ifstream& file, T(&array)[N]) {
	if (!file.read(reinterpret_cast<char*>(array), sizeof(T) * N)) {
		throw std::runtime_error("Failed to read binary array from file.");
	}
}


#pragma pack(push, 1)
struct HeaderDat {
    char magic[2];
    uint16_t version;
    uint16_t zero;
    uint16_t headerSize;
    char type[4];
};

struct ChunkHeader {
    char magic[4];
    uint32_t chunkSize;
    uint16_t version;
    uint16_t headerSize;
    uint32_t offsetToOffsetTable;
};
#pragma pack(pop)

struct ChunkData {
    ChunkHeader header;
    std::vector<uint8_t> data;
    std::vector<uint32_t> offsets;
    std::vector<uint8_t> unknown;
};

// Helper struct to track reading position
struct MemReader {
    const uint8_t* ptr;
    size_t size;

    MemReader(std::vector<uint8_t>& buffer)
        : ptr(buffer.data()), size(buffer.size()) {
    }
};

// Read POD struct from memory
template<typename T>
bool readStruct(MemReader& reader, T& outStruct) {
    if (reader.size < sizeof(T)) return false;
    std::memcpy(&outStruct, reader.ptr, sizeof(T));
    reader.ptr += sizeof(T);
    reader.size -= sizeof(T);
    return true;
}

// Read chunk from memory
bool readChunk(MemReader& reader, ChunkData& chunk);
bool parse_dat_header(Gw2Dat& data_gw2);
void print_dat_header(Gw2Dat& data_gw2);
bool parse_mft_header(Gw2Dat& data_gw2);
void print_mft_header(Gw2Dat& data_gw2);
bool parse_mft_data(Gw2Dat& data_gw2);
void print_mft_data(Gw2Dat& data_gw2, uint64_t index);
bool parse_mft_file_id_data(Gw2Dat& data_gw2);
void print_mft_file_id_data(Gw2Dat& data_gw2, uint64_t index);
bool parse_dat_file(Gw2Dat& data_gw2);
void print_file_info(Gw2Dat& data_gw2);
bool load_dat_file(Gw2Dat& data_gw2, const std::string& file_path);
std::vector<uint32_t> search_by_base_id(Gw2Dat& data_gw2, uint32_t base_id);
uint32_t get_by_base_id(Gw2Dat& data_gw2, uint32_t file_id);
std::vector<uint32_t> get_by_file_id(Gw2Dat& data_gw2, uint32_t base_id);
std::vector<uint32_t> search_by_file_id(Gw2Dat& data_gw2, uint32_t file_id);
std::vector<uint8_t> extract_compressed_data(Gw2Dat& data_gw2, uint32_t mft_index);
std::vector<uint8_t> remove_crc32c_data(Gw2Dat& data_gw2, uint32_t mft_index);
std::vector<uint8_t> convert_to_vector(const uint8_t* data, uint32_t size);
std::vector<uint8_t> extract_decompressed_data(Gw2Dat& data_gw2, uint32_t mft_index);
std::vector<uint8_t> extract_decompressed_image(std::vector<uint8_t>& uncompressed_image, gw2dt::compression::AnetImage& anet_image);



#endif // !PARSER_H
