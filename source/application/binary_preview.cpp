#include "application.h"

void render_compressed_tab(Gw2Dat& data_gw2, WindowData& window_data)
{
	// Read the compressed data buffer once
	if (window_data.dat_data.selected_base_id != window_data.dat_data.last_selected_item_compressed)
	{
		window_data.binary_data.compressed_data = extract_compressed_data(data_gw2, window_data.dat_data.selected_base_id - 1);

		window_data.dat_data.last_selected_item_compressed = window_data.dat_data.selected_base_id;
	}

	// Display compressed data
	ImGui::Text("Compressed Data (Hex):");
	ImGui::BeginChild("Compressed Scroll", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);

	ImGuiListClipper clipper;
	clipper.Begin(static_cast<int>(window_data.binary_data.compressed_data.size() / 16 + 1));

	while (clipper.Step())
	{
		for (int line = clipper.DisplayStart; line < clipper.DisplayEnd; ++line)
		{
			uint64_t start_idx = static_cast<uint64_t>(line) * 16;
			std::string line_str;

			// Line number
			char line_number[16];
			snprintf(line_number, sizeof(line_number), "%08X: ", (unsigned int)start_idx);
			line_str += line_number;

			// Hexadecimal bytes
			for (uint64_t j = 0; j < 16; ++j)
			{
				if (start_idx + j < window_data.binary_data.compressed_data.size())
				{
					char hex_byte[4];
					snprintf(hex_byte, sizeof(hex_byte), "%02X ", window_data.binary_data.compressed_data[start_idx + j]);
					line_str += hex_byte;
				}
				else
				{
					line_str += "   ";
				}
			}

			// ASCII representation
			line_str += " ";
			for (uint64_t j = 0; j < 16; ++j)
			{
				if (start_idx + j < window_data.binary_data.compressed_data.size())
				{
					char c = window_data.binary_data.compressed_data[start_idx + j];
					line_str += (c >= 32 && c <= 126) ? c : '.';
				}
				else
				{
					line_str += " ";
				}
			}

			ImGui::TextUnformatted(line_str.c_str());
		}
	}

	clipper.End();
	ImGui::EndChild();
}

void render_decompressed_tab(Gw2Dat& data_gw2, WindowData& window_data)
{
	// Read the decompressed data buffer once
	if (window_data.dat_data.selected_base_id != window_data.dat_data.last_selected_item_decompressed)
	{
		if (data_gw2.mft_data_list[window_data.dat_data.selected_base_id - 1].compression_flag != 0)
		{
			window_data.binary_data.decompressed_data = extract_decompressed_data(data_gw2, window_data.dat_data.selected_base_id - 1);
			data_gw2.mft_data_list[window_data.dat_data.selected_base_id - 1].uncompressed_size = window_data.binary_data.decompressed_data.size();
		}
		else
		{
			window_data.binary_data.decompressed_data = remove_crc32c_data(data_gw2, window_data.dat_data.selected_base_id - 1);
			data_gw2.mft_data_list[window_data.dat_data.selected_base_id - 1].uncompressed_size = window_data.binary_data.decompressed_data.size();
		}
		window_data.dat_data.last_selected_item_decompressed = window_data.dat_data.selected_base_id;
	}
	// Display decompressed data
	ImGui::Text("Decompressed Data (Hex):");
	ImGui::BeginChild("Decompressed Scroll", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);

	ImGuiListClipper clipper;
	clipper.Begin(static_cast<int>(window_data.binary_data.decompressed_data.size() / 16 + 1));

	while (clipper.Step())
	{
		for (int line = clipper.DisplayStart; line < clipper.DisplayEnd; ++line)
		{
			uint64_t start_idx = static_cast<uint64_t>(line) * 16;
			std::string line_str;

			// Line number
			char line_number[16];
			snprintf(line_number, sizeof(line_number), "%08X: ", (unsigned int)start_idx);
			line_str += line_number;

			// Hexadecimal bytes
			for (uint64_t j = 0; j < 16; ++j)
			{
				if (start_idx + j < window_data.binary_data.decompressed_data.size())
				{
					char hex_byte[4];
					snprintf(hex_byte, sizeof(hex_byte), "%02X ", window_data.binary_data.decompressed_data[start_idx + j]);
					line_str += hex_byte;
				}
				else
				{
					line_str += "   ";
				}
			}

			// ASCII representation
			line_str += " ";
			for (uint64_t j = 0; j < 16; ++j)
			{
				if (start_idx + j < window_data.binary_data.decompressed_data.size())
				{
					char c = window_data.binary_data.decompressed_data[start_idx + j];
					line_str += (c >= 32 && c <= 126) ? c : '.';
				}
				else
				{
					line_str += " ";
				}
			}

			ImGui::TextUnformatted(line_str.c_str());
		}
	}

	clipper.End();
	ImGui::EndChild();
}


