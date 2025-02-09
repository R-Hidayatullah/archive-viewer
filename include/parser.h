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

#include "gw2dattools/inflateDatFileBuffer.h"
#include "gw2dattools/inflateTextureFileBuffer.h"

constexpr size_t MFT_MAGIC_NUMBER = 4;
constexpr size_t ANET_IMAGE_MAGIC_NUMBER = 4;
constexpr size_t HEADER_IDENTIFIER = 16;
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
	char identifier[HEADER_IDENTIFIER];
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


bool parse_dat_header(Gw2Dat& data_gw2) {
	try {
		read_from_file(data_gw2.dat_file, data_gw2.dat_header.version);
		read_from_file(data_gw2.dat_file, data_gw2.dat_header.identifier);
		read_from_file(data_gw2.dat_file, data_gw2.dat_header.header_size);
		read_from_file(data_gw2.dat_file, data_gw2.dat_header.unknown_field);
		read_from_file(data_gw2.dat_file, data_gw2.dat_header.chunk_size);
		read_from_file(data_gw2.dat_file, data_gw2.dat_header.crc);
		read_from_file(data_gw2.dat_file, data_gw2.dat_header.unknown_field_2);
		read_from_file(data_gw2.dat_file, data_gw2.dat_header.mft_offset);
		read_from_file(data_gw2.dat_file, data_gw2.dat_header.mft_size);
		read_from_file(data_gw2.dat_file, data_gw2.dat_header.flag);

		// Attempt to seek into MFT offset
		if (!data_gw2.dat_file.seekg(data_gw2.dat_header.mft_offset, std::ios::beg)) {
			throw std::runtime_error("Failed to seek into MFT Offset.");
		}
		return true; // Return true if parsing is successful
	}
	catch (const std::exception& e) {
		std::cerr << "Error while parsing DAT header: " << e.what() << std::endl;
		return false; // Return false if an exception is thrown
	}
}


void print_dat_header(Gw2Dat& data_gw2) {
	std::cout << "\nDAT File Header Summary:\n"
		<< "-------------------------\n"
		<< std::left << std::setw(20) << "Version :" << std::setw(10) << static_cast<int>(data_gw2.dat_header.version) << "\n"
		<< std::setw(20) << "Identifier :" << std::setw(10) << std::string(data_gw2.dat_header.identifier, data_gw2.dat_header.identifier + DAT_MAGIC_NUMBER) << "\n"
		<< std::setw(20) << "Header Size :" << std::setw(10) << data_gw2.dat_header.header_size << " bytes\n"
		<< std::setw(20) << "Unknown Field :" << std::setw(10) << data_gw2.dat_header.unknown_field << "\n"
		<< std::setw(20) << "Chunk Size :" << std::setw(10) << data_gw2.dat_header.chunk_size << " bytes\n";

	// Print CRC as 4 bytes in hexadecimal
	std::cout << std::setw(20) << "CRC 32C :";
	for (int i = 3; i >= 0; --i) { // Iterate over the 4 bytes of the CRC field
		std::cout << std::hex << std::uppercase
			<< ((data_gw2.dat_header.crc >> (i * 8)) & 0xFF); // Extract each byte
		if (i > 0) {
			std::cout << " "; // Add space between bytes
		}
	}
	std::cout << std::dec << "\n"; // Reset to decimal formatting

	std::cout << std::setw(20) << "Unknown Field 2:" << std::setw(10) << data_gw2.dat_header.unknown_field_2 << "\n"
		<< std::setw(20) << "MFT Offset:" << std::setw(10) << data_gw2.dat_header.mft_offset << "\n"
		<< std::setw(20) << "MFT Size:" << std::setw(10) << data_gw2.dat_header.mft_size << " bytes\n"
		<< std::setw(20) << "Flag:" << std::setw(10) << data_gw2.dat_header.flag << "\n";
}

bool parse_mft_header(Gw2Dat& data_gw2) {
	try {
		read_from_file(data_gw2.dat_file, data_gw2.mft_header.identifier);
		read_from_file(data_gw2.dat_file, data_gw2.mft_header.unknown_field);
		read_from_file(data_gw2.dat_file, data_gw2.mft_header.mft_entry_size);
		read_from_file(data_gw2.dat_file, data_gw2.mft_header.unknown_field_2);
		read_from_file(data_gw2.dat_file, data_gw2.mft_header.unknown_field_3);
		return true; // Return true if parsing is successful
	}
	catch (const std::exception& e) {
		std::cerr << "Error while parsing MFT header: " << e.what() << std::endl;
		return false; // Return false if an exception is thrown
	}
}

void print_mft_header(Gw2Dat& data_gw2) {
	std::cout << "\nMFT File Header Summary:\n"
		<< "-------------------------\n"
		<< std::left << std::setw(20) << "Identifier :" << std::string(data_gw2.mft_header.identifier, data_gw2.mft_header.identifier + DAT_MAGIC_NUMBER) << "\n"
		<< std::setw(20) << "Unknown Field :" << data_gw2.mft_header.unknown_field << "\n"
		<< std::setw(20) << "MFT Entry Size :" << data_gw2.mft_header.mft_entry_size << "\n"
		<< std::setw(20) << "Unknown Field 2 :" << data_gw2.mft_header.unknown_field_2 << "\n"
		<< std::setw(20) << "Unknown Field 3 :" << data_gw2.mft_header.unknown_field_3 << "\n";
}

bool parse_mft_data(Gw2Dat& data_gw2) {
	data_gw2.mft_data_list.resize(data_gw2.mft_header.mft_entry_size - 1);

	for (uint64_t i = 0; i < data_gw2.mft_header.mft_entry_size - 1; i++) {
		try {
			data_gw2.mft_data_list[i].original_index = i;
			read_from_file(data_gw2.dat_file, data_gw2.mft_data_list[i].offset);
			read_from_file(data_gw2.dat_file, data_gw2.mft_data_list[i].size);
			read_from_file(data_gw2.dat_file, data_gw2.mft_data_list[i].compression_flag);
			read_from_file(data_gw2.dat_file, data_gw2.mft_data_list[i].entry_flag);
			read_from_file(data_gw2.dat_file, data_gw2.mft_data_list[i].counter);
			read_from_file(data_gw2.dat_file, data_gw2.mft_data_list[i].crc);
		}
		catch (const std::exception& e) {
			std::cerr << "Error while parsing MFT Data at index " << i << ": " << e.what() << std::endl;
			return false;
		}
	}

	return true; // Return true only after the whole loop finishes successfully
}


void print_mft_data(Gw2Dat& data_gw2, uint64_t index) {
	std::cout << "\nMFT Data Summary:\n"
		<< "-------------------------\n"
		<< std::left << std::setw(20) << "Index :" << std::setw(10) << data_gw2.mft_data_list[index].original_index << "\n"
		<< std::setw(20) << "Offset :" << std::setw(10) << data_gw2.mft_data_list[index].offset << "\n"
		<< std::setw(20) << "Size :" << std::setw(10) << data_gw2.mft_data_list[index].size << " bytes\n"
		<< std::setw(20) << "Compression Flag :" << std::setw(10) << data_gw2.mft_data_list[index].compression_flag << "\n"
		<< std::setw(20) << "Entry Flag :" << std::setw(10) << data_gw2.mft_data_list[index].entry_flag << "\n"
		<< std::setw(20) << "Counter :" << std::setw(10) << data_gw2.mft_data_list[index].counter << "\n";

	// Print CRC as 4 bytes in hexadecimal
	std::cout << std::setw(20) << "CRC 32C :";
	for (int i = 3; i >= 0; --i) { // Iterate over the 4 bytes of the CRC field
		std::cout << std::hex << std::uppercase
			<< ((data_gw2.mft_data_list[index].crc >> (i * 8)) & 0xFF); // Extract each byte
		if (i > 0) {
			std::cout << " "; // Add space between bytes
		}
	}
	std::cout << std::dec << "\n"
		<< std::setw(20) << "Identifier :" << std::string(data_gw2.mft_data_list[index].identifier, data_gw2.mft_data_list[index].identifier + HEADER_IDENTIFIER) << "\n"
		<< std::setw(20) << "Uncompressed Size :" << std::setw(10) << data_gw2.mft_data_list[index].uncompressed_size << " bytes\n"; // Reset to decimal formatting
}

bool parse_mft_file_id_data(Gw2Dat& data_gw2) {
	// Ensure the index is valid
	if (MFT_ENTRY_INDEX_NUM >= data_gw2.mft_data_list.size()) {
		std::cerr << "Error: MFT_ENTRY_INDEX_NUM is out of bounds." << std::endl;
		return false;
	}

	// Get the data size and offset
	uint64_t entry_size = data_gw2.mft_data_list[MFT_ENTRY_INDEX_NUM].size;
	uint64_t entry_offset = data_gw2.mft_data_list[MFT_ENTRY_INDEX_NUM].offset;

	// Calculate the number of entries
	size_t num_entries = entry_size / sizeof(MftFileIdData);
	data_gw2.mft_file_id_data_list.resize(num_entries);

	// Seek to the correct position in the file
	data_gw2.dat_file.seekg(entry_offset, std::ios::beg);
	if (!data_gw2.dat_file) {
		std::cerr << "Error: Failed to seek to MFT file ID data offset." << std::endl;
		return false;
	}

	// Read data
	for (size_t i = 0; i < num_entries; i++) {
		try {
			read_from_file(data_gw2.dat_file, data_gw2.mft_file_id_data_list[i].file_id);
			read_from_file(data_gw2.dat_file, data_gw2.mft_file_id_data_list[i].base_id);
		}
		catch (const std::exception& e) {
			std::cerr << "Error while parsing MFT File ID Data at index " << i << ": " << e.what() << std::endl;
			return false;
		}
	}

	std::sort(data_gw2.mft_file_id_data_list.begin(), data_gw2.mft_file_id_data_list.end(), [](const MftFileIdData& a, const MftFileIdData& b) {
		return a.file_id < b.file_id;
		});

	std::unordered_map<uint32_t, std::vector<uint32_t>> list_mft_base_id;

	for (const auto& mft_file_id : data_gw2.mft_file_id_data_list) {
		list_mft_base_id[mft_file_id.base_id].push_back(mft_file_id.file_id);
	}

	for (const auto& mft_base_id : list_mft_base_id) {
		MftBaseIdData entry{};
		entry.base_id = mft_base_id.first;
		entry.file_id = mft_base_id.second;
		data_gw2.mft_base_id_data_list.push_back(std::move(entry));
	}


	std::sort(data_gw2.mft_base_id_data_list.begin(), data_gw2.mft_base_id_data_list.end(), [](const MftBaseIdData& a, const MftBaseIdData& b) {
		return a.base_id < b.base_id;
		});

	return true; // Return true if parsing is successful
}


void print_mft_file_id_data(Gw2Dat& data_gw2, uint64_t index) {
	std::cout << "\nMFT File Id Data Summary:\n"
		<< "-------------------------\n"
		<< std::left << std::setw(20) << "File Id :" << std::setw(10) << data_gw2.mft_file_id_data_list[index].file_id << "\n"
		<< std::setw(20) << "Base Id :" << std::setw(10) << data_gw2.mft_file_id_data_list[index].base_id << "\n";
}


bool parse_dat_file(Gw2Dat& data_gw2) {
	if (!parse_dat_header(data_gw2)) {
		std::cerr << "Error: Failed to parse DAT header." << std::endl;
		return false;
	}
	print_dat_header(data_gw2);

	if (!parse_mft_header(data_gw2)) {
		std::cerr << "Error: Failed to parse MFT header." << std::endl;
		return false;
	}
	print_mft_header(data_gw2);

	if (!parse_mft_data(data_gw2)) {
		std::cerr << "Error: Failed to parse MFT data." << std::endl;
		return false;
	}
	print_mft_data(data_gw2, 0);

	if (!parse_mft_file_id_data(data_gw2)) {
		std::cerr << "Error: Failed to parse MFT file ID data." << std::endl;
		return false;
	}
	print_mft_file_id_data(data_gw2, 0);

	return true;
}


void print_file_info(Gw2Dat& data_gw2) {
	std::cout << "File Info\n"
		<< "File path : " << data_gw2.file_info.file_path << "\n"
		<< "File size : " << data_gw2.file_info.file_size << " bytes\n";
}


bool load_dat_file(Gw2Dat& data_gw2, const std::string& file_path) {
	try {
		// Open file in binary mode, positioning at the end to get file size
		data_gw2.dat_file.open(file_path, std::ios::binary | std::ios::ate);
		if (!data_gw2.dat_file.is_open()) {
			std::cerr << "Error: Failed to open file: " << file_path << std::endl;
			return false;
		}

		// Set file information
		data_gw2.file_info.file_path = file_path;
		data_gw2.file_info.file_size = data_gw2.dat_file.tellg();
		data_gw2.dat_file.seekg(0, std::ios::beg);

		// Print file info
		print_file_info(data_gw2);

		// Parse the DAT file
		if (!parse_dat_file(data_gw2)) {
			std::cerr << "Error: Failed to parse DAT file: " << file_path << std::endl;
			data_gw2.dat_file.close(); // Ensure file is closed on failure
			return false;
		}

		return true; // Successfully loaded and parsed
	}
	catch (const std::exception& e) {
		std::cerr << "Exception occurred while loading DAT file: " << e.what() << std::endl;
		data_gw2.dat_file.close(); // Ensure file is closed if an exception occurs
		return false;
	}
}

std::vector<uint32_t> search_by_base_id(Gw2Dat& data_gw2, uint32_t base_id) {
	std::vector<uint32_t> list_base_id;

	// Length of the number to find
	uint32_t divisor = 1;
	uint32_t temp = base_id;
	while (temp > 0) {
		divisor *= 10;
		temp /= 10;
	}

	// Search for the number
	for (uint32_t i = 0; i < data_gw2.mft_base_id_data_list.size(); i++) {
		uint32_t num = data_gw2.mft_base_id_data_list[i].base_id;
		while (num >= base_id) { // Only check if num is large enough
			if (num % divisor == base_id) { // Match the substring
				list_base_id.push_back(data_gw2.mft_base_id_data_list[i].base_id);
				break;
			}
			num /= 10; // Remove the last digit
		}

	}

	std::sort(list_base_id.begin(), list_base_id.end());

	return list_base_id;
}

uint32_t get_by_base_id(Gw2Dat& data_gw2, uint32_t file_id) {
	uint32_t data_found = 0;

	// Search for the number
	for (uint32_t i = 0; i < data_gw2.mft_file_id_data_list.size(); i++) {
		uint32_t num = data_gw2.mft_file_id_data_list[i].file_id;
		if (num == file_id)
		{
			data_found = data_gw2.mft_file_id_data_list[i].base_id;
			break;
		}
	}

	return data_found;
}

std::vector<uint32_t> get_by_file_id(Gw2Dat& data_gw2, uint32_t base_id) {
	std::vector<uint32_t> list_file_id;

	for (uint32_t i = 0;i < data_gw2.mft_base_id_data_list.size();i++) {
		uint32_t num = data_gw2.mft_base_id_data_list[i].base_id;
		if (num == base_id)
		{
			list_file_id = data_gw2.mft_base_id_data_list[i].file_id;
			break;
		}
	}

	std::sort(list_file_id.begin(), list_file_id.end());

	return list_file_id;
}


std::vector<uint32_t> search_by_file_id(Gw2Dat& data_gw2, uint32_t file_id) {
	std::vector<uint32_t> list_file_id;

	// Length of the number to find
	uint32_t divisor = 1;
	uint32_t temp = file_id;
	while (temp > 0) {
		divisor *= 10;
		temp /= 10;
	}

	// Search for the number
	for (uint32_t i = 0; i < data_gw2.mft_file_id_data_list.size(); i++) {

		uint32_t num = data_gw2.mft_file_id_data_list[i].file_id;
		while (num >= file_id) { // Only check if num is large enough
			if (num % divisor == file_id) { // Match the substring
				list_file_id.push_back(data_gw2.mft_file_id_data_list[i].file_id);
				break;
			}
			num /= 10; // Remove the last digit
		}
	}
	std::sort(list_file_id.begin(), list_file_id.end());
	return list_file_id;
}


std::vector<uint8_t> extract_compressed_data(Gw2Dat& data_gw2, uint32_t mft_index) {
	if (mft_index >= data_gw2.mft_data_list.size()) {
		throw std::out_of_range("MFT index out of range: " + std::to_string(mft_index));
	}

	const auto& mft_entry = data_gw2.mft_data_list[mft_index];

	if (mft_entry.size == 0) {
		throw std::runtime_error("Invalid data size at index: " + std::to_string(mft_index));
	}

	std::vector<uint8_t> compressed_data(mft_entry.size);
	data_gw2.dat_file.seekg(mft_entry.offset);
	data_gw2.dat_file.read(reinterpret_cast<char*>(compressed_data.data()), mft_entry.size);

	// Check if full data was read
	if (data_gw2.dat_file.gcount() != static_cast<std::streamsize>(mft_entry.size)) {
		throw std::runtime_error("Failed to read full size at offset: " + std::to_string(mft_entry.offset));
	}

	return compressed_data;
}

std::vector<uint8_t> remove_crc32c_data(Gw2Dat& data_gw2, uint32_t mft_index) {
	std::vector<uint8_t> compressed_data = extract_compressed_data(data_gw2, mft_index);

	const auto& mft_entry = data_gw2.mft_data_list[mft_index];

	if (mft_entry.size > CHUNK_SIZE) {
		uint64_t position = 0;
		while (position + CHUNK_SIZE <= compressed_data.size()) {
			compressed_data.erase(compressed_data.begin() + position + START_INDEX,
				compressed_data.begin() + position + END_INDEX);
			position += CHUNK_SIZE - 4;
		}

		// Remove trailing CRC if present
		if (compressed_data.size() > 4) {
			compressed_data.erase(compressed_data.end() - 4, compressed_data.end());
		}
	}
	else if (mft_entry.size == CHUNK_SIZE) {
		compressed_data.erase(compressed_data.begin() + START_INDEX, compressed_data.begin() + END_INDEX);
	}
	else if (compressed_data.size() > 4) {
		compressed_data.erase(compressed_data.end() - 4, compressed_data.end());
	}

	return compressed_data;
}

std::vector<uint8_t> convert_to_vector(const uint8_t* data, uint32_t size) {
	if (!data || size == 0) {
		throw std::runtime_error("Invalid input data for vector conversion");
	}
	return std::vector<uint8_t>(data, data + size);
}

std::vector<uint8_t> extract_decompressed_data(Gw2Dat& data_gw2, uint32_t mft_index) {
	std::vector<uint8_t> cleaned_data = remove_crc32c_data(data_gw2, mft_index);

	uint32_t output_data_size = 0;
	uint8_t* decompressed_data = gw2dt::compression::inflate_dat_file_buffer(cleaned_data.size(),
		cleaned_data.data(),
		output_data_size);

	if (!decompressed_data) {
		throw std::runtime_error("Decompression data failed at index: " + std::to_string(mft_index));
	}

	std::vector<uint8_t> result_data = convert_to_vector(decompressed_data, output_data_size);

	free(decompressed_data);

	return result_data;
}


std::vector<uint8_t> extract_decompressed_image(std::vector<uint8_t>& uncompressed_image, gw2dt::compression::AnetImage& anet_image) {

	uint32_t output_data_size = 0;
	uint8_t* decompressed_image = gw2dt::compression::inflate_texture_file_buffer(uncompressed_image.size(),
		uncompressed_image.data(),
		output_data_size,
		anet_image);
	if (!decompressed_image) {
		throw std::runtime_error("Decompression image failed!");
	}
	std::vector<uint8_t> result_data = convert_to_vector(decompressed_image, output_data_size);

	free(decompressed_image);

	return result_data;
}

#endif // !PARSER_H
