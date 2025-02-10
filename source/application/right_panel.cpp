#include "application.h"

void display_header_info(Gw2Dat& data_gw2)
{
	ImGui::Begin("Header Information");
	ImGui::PushTextWrapPos();

	const auto& dat_header = data_gw2.dat_header;
	const auto& mft_header = data_gw2.mft_header;

	ImGui::Text("Filename: %s", data_gw2.file_info.file_path.c_str());
	ImGui::Text("File Size: %llu bytes", data_gw2.file_info.file_size);
	ImGui::Text("Version: %d", dat_header.version);
	ImGui::Text("Chunk Size: %u bytes", dat_header.chunk_size);
	ImGui::Text("MFT Offset: %llu", dat_header.mft_offset);
	ImGui::Text("MFT Size: %u bytes", dat_header.mft_size);

	ImGui::Separator();
	ImGui::Text("MFT Header:");
	ImGui::Text("Identifier: %.*s", MFT_MAGIC_NUMBER, mft_header.identifier);
	ImGui::Text("Entry Count: %u", mft_header.mft_entry_size);
	ImGui::Text("MFT Entry Count: %u", data_gw2.mft_data_list.size());
	ImGui::Text("Base Id Entry Count: %u", data_gw2.mft_base_id_data_list.size());
	ImGui::Text("File Id Entry Count: %u", data_gw2.mft_file_id_data_list.size());
	ImGui::PopTextWrapPos();

	ImGui::End();
}

void display_status_message(WindowData& window_data)
{
	if (!window_data.dat_data.status_message.empty() && window_data.dat_data.status_message_timer > 0.0f)
	{
		ImGui::Separator();
		ImGui::PushTextWrapPos();

		ImGui::Text("%s", window_data.dat_data.status_message.c_str());

		window_data.dat_data.status_message_timer -= ImGui::GetIO().DeltaTime; // Decrease the timer
		if (window_data.dat_data.status_message_timer <= 0.0f)
		{
			window_data.dat_data.status_message.clear(); // Clear the message when the timer expires
		}
		ImGui::PopTextWrapPos();
	}
}

void export_data_to_file(const std::string& filename, const std::vector<uint8_t>& data)
{
	std::ofstream file(filename, std::ios::binary);
	if (!file)
	{
		throw std::runtime_error("Failed to open file for writing: " + filename);
	}
	file.write(reinterpret_cast<const char*>(data.data()), data.size());
	file.close();
}

void display_selected_mft_data(Gw2Dat& data_gw2, WindowData& window_data)
{
	ImGui::Begin("Selected MFT Entry Information");
	ImGui::PushTextWrapPos();

	const auto& selected_entry = data_gw2.mft_data_list[window_data.dat_data.selected_base_id - 1];

	ImGui::Separator();
	ImGui::Text("Selected MFT Entry:");
	ImGui::Text("Index: %llu", selected_entry.original_index);
	ImGui::Text("Offset: %llu", selected_entry.offset);
	ImGui::Text("Size: %u bytes", selected_entry.size);
	ImGui::Text("Compression Flag: %u", selected_entry.compression_flag);
	ImGui::Text("Entry Flag: %u", selected_entry.entry_flag);
	ImGui::Text("Counter: %u", selected_entry.counter);
	ImGui::Text("CRC: %u", selected_entry.crc);
	ImGui::Text("Uncompressed Size: %u bytes", selected_entry.uncompressed_size);

	ImGui::Separator();
	ImGui::Text("Export Compressed Data into binary file");

	if (ImGui::Button("Export Compressed Data"))
	{
		try
		{
			window_data.binary_data.compressed_data = extract_compressed_data(data_gw2, window_data.dat_data.selected_base_id - 1);
			std::string filename = "compressed_" + std::to_string(window_data.dat_data.selected_base_id) + ".bin";
			export_data_to_file(filename, window_data.binary_data.compressed_data);
			window_data.dat_data.status_message = "Compressed data exported to " + filename;
		}
		catch (const std::exception& e)
		{
			window_data.dat_data.status_message = std::string("Error: ") + e.what();
		}
	}
	ImGui::Text("Export Decompressed Data into binary file");

	if (ImGui::Button("Export Decompressed Data"))
	{
		try
		{

			if (selected_entry.compression_flag != 0)
			{
				window_data.binary_data.decompressed_data = extract_decompressed_data(data_gw2, window_data.dat_data.selected_base_id - 1);

			}
			else
			{
				window_data.binary_data.decompressed_data = remove_crc32c_data(data_gw2, window_data.dat_data.selected_base_id - 1);

			}
			std::string filename = "decompressed_" + std::to_string(window_data.dat_data.selected_base_id) + ".bin";
			export_data_to_file(filename, window_data.binary_data.decompressed_data);
			window_data.dat_data.status_message = "Decompressed data exported to " + filename;
		}
		catch (const std::exception& e)
		{
			window_data.dat_data.status_message = std::string("Error: ") + e.what();
		}
	}

	// Render status message
	display_status_message(window_data);


	ImGui::PopTextWrapPos();

	ImGui::End();
}

void render_right_panel(Gw2Dat& data_gw2, WindowData& window_data)
{
	ImGui::Separator();

	if (!data_gw2.mft_data_list.empty())
	{

		// Render file header information
		display_header_info(data_gw2);
		display_selected_mft_data(data_gw2, window_data);
	}
	else
	{
		ImGui::Text("No DAT file loaded.");
	}


}