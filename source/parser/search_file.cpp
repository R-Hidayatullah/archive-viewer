#include "parser.h"

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