#include "parser.h"

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