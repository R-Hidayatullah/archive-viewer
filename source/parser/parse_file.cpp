#include "parser.h"

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

	//// Sort and remove duplicates in mftDataList_
	//std::sort(data_gw2.mft_data_list.begin(), data_gw2.mft_data_list.end(), [](const MftData& a, const MftData& b) {
	//	return a.offset < b.offset;
	//	});


	//for (uint64_t i = 0; i < data_gw2.mft_data_list.size(); ++i) {
	//	if (data_gw2.mft_data_list[i].size != 0) {
	//		if (!data_gw2.dat_file.seekg(data_gw2.mft_data_list[i].offset + 4, std::ios::beg) || !data_gw2.dat_file) {
	//			throw std::runtime_error("Failed to seek into MFT Offset.");
	//		}
	//		read_from_file(data_gw2.dat_file, data_gw2.mft_data_list[i].uncompressed_size);

	//	}
	//}

	//// Sort and remove duplicates in data_gw2.mft_data_list
	//std::sort(data_gw2.mft_data_list.begin(), data_gw2.mft_data_list.end(), [](const MftData& a, const MftData& b) {
	//	return a.original_index < b.original_index;
	//	});

	for (uint64_t i = 0;i < data_gw2.mft_data_list.size();i++) {
		uint32_t uncompressed_size = data_gw2.mft_data_list[i].size;

		if (data_gw2.mft_data_list[i].compression_flag == 0) {
			if (data_gw2.mft_data_list[i].size > CHUNK_SIZE)
			{
				uncompressed_size = uncompressed_size - (((data_gw2.mft_data_list[i].size % CHUNK_SIZE) + 1) * 4);

			}
			else if (data_gw2.mft_data_list[i].size == CHUNK_SIZE)
			{
				uncompressed_size = uncompressed_size - (1 * 4);

			}
			else if (data_gw2.mft_data_list[i].size < CHUNK_SIZE) {
				uncompressed_size = uncompressed_size - (1 * 4);
			}
			data_gw2.mft_data_list[i].uncompressed_size = std::move(uncompressed_size);
		}

	}


	return true; // Return true only after the whole loop finishes successfully
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


