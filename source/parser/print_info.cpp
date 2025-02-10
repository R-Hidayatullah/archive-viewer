#include "parser.h"

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

void print_mft_header(Gw2Dat& data_gw2) {
	std::cout << "\nMFT File Header Summary:\n"
		<< "-------------------------\n"
		<< std::left << std::setw(20) << "Identifier :" << std::string(data_gw2.mft_header.identifier, data_gw2.mft_header.identifier + DAT_MAGIC_NUMBER) << "\n"
		<< std::setw(20) << "Unknown Field :" << data_gw2.mft_header.unknown_field << "\n"
		<< std::setw(20) << "MFT Entry Size :" << data_gw2.mft_header.mft_entry_size << "\n"
		<< std::setw(20) << "Unknown Field 2 :" << data_gw2.mft_header.unknown_field_2 << "\n"
		<< std::setw(20) << "Unknown Field 3 :" << data_gw2.mft_header.unknown_field_3 << "\n";
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
		<< std::setw(20) << "Uncompressed Size :" << std::setw(10) << data_gw2.mft_data_list[index].uncompressed_size << " bytes\n"; // Reset to decimal formatting
}

void print_mft_file_id_data(Gw2Dat& data_gw2, uint64_t index) {
	std::cout << "\nMFT File Id Data Summary:\n"
		<< "-------------------------\n"
		<< std::left << std::setw(20) << "File Id :" << std::setw(10) << data_gw2.mft_file_id_data_list[index].file_id << "\n"
		<< std::setw(20) << "Base Id :" << std::setw(10) << data_gw2.mft_file_id_data_list[index].base_id << "\n";
}

void print_file_info(Gw2Dat& data_gw2) {
	std::cout << "File Info:\n"
		<< "-------------------------\n"
		<< "File path : " << data_gw2.file_info.file_path << "\n"
		<< "File size : " << data_gw2.file_info.file_size << " bytes\n";
}