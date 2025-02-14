#include "application.h"


void render_preview_tab(Gw2Dat& data_gw2, WindowData& window_data) {

	// Display preview data
	ImGui::Text("Preview Data:");

	if (window_data.dat_data.selected_base_id != window_data.dat_data.last_selected_video_decompressed)
	{
		cleanup_bink(window_data);
		window_data.dat_data.last_selected_video_decompressed = window_data.dat_data.selected_base_id;
	}



	if (window_data.dat_data.selected_base_id != window_data.dat_data.last_selected_item_decompressed)
	{
		if (data_gw2.mft_data_list[window_data.dat_data.selected_base_id - 1].compression_flag != 0)
		{
			window_data.binary_data.decompressed_data = extract_decompressed_data(data_gw2, window_data.dat_data.selected_base_id - 1);
			data_gw2.mft_data_list[window_data.dat_data.selected_base_id - 1].uncompressed_size = window_data.binary_data.decompressed_data.size();
		}
		else {
			window_data.binary_data.decompressed_data = remove_crc32c_data(data_gw2, window_data.dat_data.selected_base_id - 1);
			data_gw2.mft_data_list[window_data.dat_data.selected_base_id - 1].uncompressed_size = window_data.binary_data.decompressed_data.size();

		}
		window_data.dat_data.last_selected_item_decompressed = window_data.dat_data.selected_base_id;

	}

	if (window_data.dat_data.selected_base_id != window_data.dat_data.last_selected_image_decompressed)
	{
		if (valid_atex(window_data.binary_data.decompressed_data.data(), window_data.binary_data.decompressed_data.size()) ||
			valid_ateu(window_data.binary_data.decompressed_data.data(), window_data.binary_data.decompressed_data.size()) ||
			valid_atep(window_data.binary_data.decompressed_data.data(), window_data.binary_data.decompressed_data.size()))
		{
			window_data.binary_data.decompressed_image = extract_decompressed_image(window_data.binary_data.decompressed_data, window_data.anet_image);
		}
		window_data.dat_data.last_selected_image_decompressed = window_data.dat_data.selected_base_id;

	}
	// Check file header for supported formats
	const uint8_t* data_ptr = reinterpret_cast<const uint8_t*>(window_data.binary_data.decompressed_data.data());
	size_t data_size = window_data.binary_data.decompressed_data.size();
	if (check_valid_image(data_ptr, data_size))
	{
		render_image(window_data, data_ptr, data_size);
	}
	else if (valid_bink2(data_ptr, data_size))
	{
		render_video(window_data, data_ptr, data_size);
	}
	else if (valid_pf_modl(data_ptr, data_size))
	{
		render_model(data_gw2, window_data, data_ptr, data_size);
	}
}


void render_middle_panel(Gw2Dat& data_gw2, WindowData& window_data)
{
	ImGui::Begin("Extracted Data");

	if (ImGui::BeginTabBar("MFT Data Tabs"))
	{
		window_data.preview_tab_active = false;  // Reset flag before checking active tab

		if (ImGui::BeginTabItem("Compressed"))
		{
			render_compressed_tab(data_gw2, window_data);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Decompressed"))
		{
			render_decompressed_tab(data_gw2, window_data);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Preview"))
		{
			if (ImGui::IsItemFocused())
			{

				if (!window_data.preview_tab_active) {
					window_data.preview_tab_active = true;
				}
			}
			render_preview_tab(data_gw2, window_data);
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}