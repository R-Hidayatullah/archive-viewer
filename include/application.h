#ifndef APPLICATION_H
#define APPLICATION_H

#include "parser.h"
#define GL_SILENCE_DEPRECATION
#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <chrono>
#include <thread>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "glad/glad.h"

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <webp/decode.h>
#include <d3d11.h>
#include <dxgi.h>
#include <DirectXTex.h>
#include <png.h>
#include <jpeglib.h>

#include "bink_2_7d.h"

struct BinaryData {
	std::vector<uint8_t> decompressed_data;
	std::vector<uint8_t> decompressed_image;
	std::vector<uint8_t> compressed_data;
};

struct ImageData {
	unsigned char* image_data;
	GLuint texture_id;
	int image_width;
	int image_height;
	int image_channel;
	bool anet_image;
	uint32_t format_data;

};

struct VideoData {
	HBINK bink_handler;
	GLuint video_texture;
	bool is_playing;
	bool first_frame;
	float current_time;
	float total_time;
	float seek_time;
	float last_frametime;
	float actual_framerate;
};

struct DatData {
	int search_number;
	int temp_number;
	uint64_t selected_base_id;
	uint64_t selected_file_id;
	bool show_by_base_id;
	std::vector<uint32_t> found_results;
	float status_message_timer;
	std::string status_message;
	uint64_t last_selected_item_compressed;
	uint64_t last_selected_item_decompressed;
	uint64_t last_selected_video_decompressed;
	uint64_t last_selected_image_decompressed;
};

struct WindowData {
	GLFWwindow* window;
	std::string window_title;
	ImGuiDockNodeFlags dockspace_flags;
	DatData dat_data;
	VideoData video_data;
	ImageData image_data;
	BinaryData binary_data;
	gw2dt::compression::AnetImage anet_image;
};

constexpr ImVec4 CLEAR_COLOR(0.45f, 0.55f, 0.60f, 1.00f);


// https://github.com/ocornut/imgui/issues/707
// https://github.com/codz01
void flat_style()
{

	ImGuiStyle& style = ImGui::GetStyle();
	ImGui::StyleColorsDark(&style);
	style.WindowRounding = 5.3f;
	style.FrameRounding = 2.3f;
	style.ScrollbarRounding = 0;

	style.Colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 0.90f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.09f, 0.09f, 0.15f, 1.00f);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.05f, 0.10f, 0.85f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.70f, 0.70f, 0.70f, 0.65f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.00f, 0.00f, 0.01f, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.90f, 0.80f, 0.80f, 0.40f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.90f, 0.65f, 0.65f, 0.45f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.83f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.40f, 0.40f, 0.80f, 0.20f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 0.00f, 0.00f, 0.87f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.01f, 0.01f, 0.02f, 0.80f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.25f, 0.30f, 0.60f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.55f, 0.53f, 0.55f, 0.51f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.56f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.91f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.90f, 0.90f, 0.90f, 0.83f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.70f, 0.70f, 0.70f, 0.62f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.30f, 0.30f, 0.30f, 0.84f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.48f, 0.72f, 0.89f, 0.49f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.50f, 0.69f, 0.99f, 0.68f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.80f, 0.50f, 0.50f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.30f, 0.69f, 1.00f, 0.53f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.44f, 0.61f, 0.86f, 1.00f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.38f, 0.62f, 0.83f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.85f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
}

// Function to set a dark theme
void set_dark_theme()
{
	ImGui::StyleColorsDark();
}

// Function to set a light theme
void set_light_theme()
{
	ImGui::StyleColorsLight();
}

// Function to set a classic theme
void set_classic_theme()
{
	ImGui::StyleColorsClassic();
}

void set_flat_theme()
{
	flat_style();
}


void render_menu_bar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Themes"))
		{
			if (ImGui::MenuItem("Dark Theme"))
			{
				set_dark_theme();
			}
			if (ImGui::MenuItem("Light Theme"))
			{
				set_light_theme();
			}
			if (ImGui::MenuItem("Classic Theme"))
			{
				set_classic_theme();
			}
			if (ImGui::MenuItem("Flat Dark Theme"))
			{
				set_flat_theme();
			}

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

void render_left_panel(Gw2Dat& data_gw2, WindowData& window_data) {
	ImGui::Begin("MFT Data");

	ImGui::Text("Search Bar:");
	ImGui::InputInt("##SearchBar", &window_data.dat_data.search_number);
	ImGui::Separator();
	ImGui::Checkbox("Show By Base Id", &window_data.dat_data.show_by_base_id);
	ImGui::Separator();
	ImGui::Text("MFT Data List:");

	if (window_data.dat_data.show_by_base_id)
	{
		ImGui::Text("Base Id | File Id");
	}
	else
	{
		ImGui::Text("File Id | Base Id");
	}
	ImVec2 child_size = ImVec2(0, 0); // Adjust height as needed
	ImGui::BeginChild("MFTList", child_size, true, ImGuiWindowFlags_HorizontalScrollbar);

	if (window_data.dat_data.show_by_base_id)
	{
		if (window_data.dat_data.search_number > 0)
		{

			if (window_data.dat_data.temp_number != window_data.dat_data.search_number)
			{
				window_data.dat_data.found_results = search_by_base_id(data_gw2, window_data.dat_data.search_number);
				window_data.dat_data.temp_number = window_data.dat_data.search_number;
			}
			const auto& mft_base_data = data_gw2.mft_base_id_data_list;

			size_t total_items = window_data.dat_data.found_results.size();
			// Use ImGuiListClipper for efficient rendering of large lists
			ImGuiListClipper clipper;
			clipper.Begin(static_cast<int>(total_items)); // Total number of items in the list

			while (clipper.Step())
			{
				for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
				{
					uint64_t base_id = window_data.dat_data.found_results[i];
					// Construct a unique label for each selectable item
					std::string label = std::to_string(base_id) + " | [";
					// Loop through the file_id_ vector and add each element to the label string
					const auto& file_id = get_by_file_id(data_gw2, base_id);

					for (size_t j = 0; j < file_id.size(); ++j)
					{
						label += std::to_string(file_id[j]);
						if (j != file_id.size() - 1)
						{
							label += ", ";
						}
					}
					label += "]";

					// Display the selectable item
					if (ImGui::Selectable(label.c_str(), window_data.dat_data.selected_base_id == base_id))
					{
						// Update the selected index when clicked
						window_data.dat_data.selected_base_id = base_id;
					}

					// Handle copying with Ctrl + C
					if (ImGui::IsItemHovered() && ImGui::IsKeyPressed(ImGuiKey_C) && ImGui::GetIO().KeyCtrl)
					{
						// Copy the label text to the clipboard
						ImGui::SetClipboardText(label.c_str());
					}
				}
			}

			clipper.End();
		}

		if (window_data.dat_data.search_number == 0)
		{
			// Retrieve the MFT base data
			const auto& mft_base_data = data_gw2.mft_base_id_data_list;
			size_t total_items = mft_base_data.size();

			// Ensure there are items to display
			if (total_items == 0)
			{
				ImGui::Text("No entries available.");
				return;
			}

			// Use ImGuiListClipper for efficient rendering of large lists
			ImGuiListClipper clipper;
			clipper.Begin(static_cast<uint64_t>(total_items)); // Total number of items in the list

			while (clipper.Step())
			{
				for (uint64_t i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
				{
					// Construct a unique label for each selectable item
					std::string label = std::to_string(mft_base_data[i].base_id) + " | [";

					// Loop through the file_id_ vector and add each element to the label string
					const auto& file_id = mft_base_data[i].file_id;
					for (size_t j = 0; j < file_id.size(); ++j)
					{
						label += std::to_string(file_id[j]);
						if (j != file_id.size() - 1)
						{
							label += ", ";
						}
					}

					label += "]";

					// Display the selectable item
					if (ImGui::Selectable(label.c_str(), window_data.dat_data.selected_base_id == mft_base_data[i].base_id))
					{
						// Update the selected index when clicked
						window_data.dat_data.selected_base_id = mft_base_data[i].base_id;
					}

					// Handle copying with Ctrl + C
					if (ImGui::IsItemHovered() && ImGui::IsKeyPressed(ImGuiKey_C) && ImGui::GetIO().KeyCtrl)
					{
						// Copy the label text to the clipboard
						ImGui::SetClipboardText(label.c_str());
					}
				}
			}

			clipper.End();
		}
	}
	else
	{

		if (window_data.dat_data.search_number > 0)
		{

			if (window_data.dat_data.temp_number != window_data.dat_data.search_number)
			{
				window_data.dat_data.found_results = search_by_file_id(data_gw2, window_data.dat_data.search_number);

				window_data.dat_data.temp_number = window_data.dat_data.search_number;
			}

			size_t total_items = window_data.dat_data.found_results.size();
			// Use ImGuiListClipper for efficient rendering of large lists
			ImGuiListClipper clipper;
			clipper.Begin(static_cast<int>(total_items)); // Total number of items in the list

			while (clipper.Step())
			{
				for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
				{
					uint64_t file_id = window_data.dat_data.found_results[i];
					uint64_t base_id = static_cast<uint64_t>(get_by_base_id(data_gw2, file_id));

					// Construct a unique label for each selectable item
					std::string label = std::to_string(file_id) + " | " + std::to_string(base_id);

					// Display the selectable item
					if (ImGui::Selectable(label.c_str(), window_data.dat_data.selected_base_id == base_id))
					{
						// Update the selected index when clicked
						window_data.dat_data.selected_base_id = base_id;
						window_data.dat_data.selected_file_id = file_id;
					}

					// Handle copying with Ctrl + C
					if (ImGui::IsItemHovered() && ImGui::IsKeyPressed(ImGuiKey_C) && ImGui::GetIO().KeyCtrl)
					{
						// Copy the label text to the clipboard
						ImGui::SetClipboardText(label.c_str());
					}
				}
			}

			clipper.End();
		}

		if (window_data.dat_data.search_number == 0)
		{
			// Retrieve the MFT file data
			const auto& mft_file_data = data_gw2.mft_file_id_data_list;

			size_t total_items = mft_file_data.size();

			// Ensure there are items to display
			if (total_items == 0)
			{
				ImGui::Text("No entries available.");
				return;
			}

			// Use ImGuiListClipper for efficient rendering of large lists
			ImGuiListClipper clipper;
			clipper.Begin(static_cast<int>(total_items)); // Total number of items in the list

			while (clipper.Step())
			{
				for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
				{
					// Construct a unique label for each selectable item
					std::string label = std::to_string(mft_file_data[i].file_id) + " | " + std::to_string(mft_file_data[i].base_id);

					// Display the selectable item
					if (ImGui::Selectable(label.c_str(), window_data.dat_data.selected_base_id == mft_file_data[i].base_id))
					{
						// Update the selected index when clicked
						window_data.dat_data.selected_base_id = mft_file_data[i].base_id;
						window_data.dat_data.selected_file_id = mft_file_data[i].file_id;
					}

					// Handle copying with Ctrl + C
					if (ImGui::IsItemHovered() && ImGui::IsKeyPressed(ImGuiKey_C) && ImGui::GetIO().KeyCtrl)
					{
						// Copy the label text to the clipboard
						ImGui::SetClipboardText(label.c_str());
					}
				}
			}

			clipper.End();
		}
	}
	ImGui::EndChild();
	ImGui::End();
}

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

bool valid_png(const uint8_t* data_ptr, size_t data_size)
{
	return (data_size >= 8 &&
		data_ptr[0] == 0x89 && data_ptr[1] == 0x50 &&
		data_ptr[2] == 0x4E && data_ptr[3] == 0x47 &&
		data_ptr[4] == 0x0D && data_ptr[5] == 0x0A &&
		data_ptr[6] == 0x1A && data_ptr[7] == 0x0A);
}

bool valid_jpeg(const uint8_t* data_ptr, size_t data_size)
{
	return (data_size >= 3 &&
		data_ptr[0] == 0xFF && data_ptr[1] == 0xD8 &&
		data_ptr[2] == 0xFF);
}

bool valid_webp(const uint8_t* data_ptr, size_t data_size)
{
	return (data_size >= 12 &&
		data_ptr[0] == 0x52 && data_ptr[1] == 0x49 &&
		data_ptr[2] == 0x46 && data_ptr[3] == 0x46); // "RIFF"
}

bool valid_dds(const uint8_t* data_ptr, size_t data_size)
{
	return (data_size >= 4 &&
		data_ptr[0] == 'D' && data_ptr[1] == 'D' &&
		data_ptr[2] == 'S' && data_ptr[3] == ' '); // "DDS "
}

bool valid_atex(const uint8_t* data_ptr, size_t data_size)
{
	return (data_size >= 4 &&
		data_ptr[0] == 'A' && data_ptr[1] == 'T' &&
		data_ptr[2] == 'E' && data_ptr[3] == 'X'); // "ATEX"
}

bool valid_ateu(const uint8_t* data_ptr, size_t data_size)
{
	return (data_size >= 4 &&
		data_ptr[0] == 'A' && data_ptr[1] == 'T' &&
		data_ptr[2] == 'E' && data_ptr[3] == 'U'); // "ATEU"
}

bool valid_atep(const uint8_t* data_ptr, size_t data_size)
{
	return (data_size >= 4 &&
		data_ptr[0] == 'A' && data_ptr[1] == 'T' &&
		data_ptr[2] == 'E' && data_ptr[3] == 'P'); // "ATEP"
}

bool valid_ctex(const uint8_t* data_ptr, size_t data_size)
{
	return (data_size >= 4 &&
		data_ptr[0] == 'C' && data_ptr[1] == 'T' &&
		data_ptr[2] == 'E' && data_ptr[3] == 'X'); // "CTEX"
}

bool check_valid_image(const uint8_t* data_ptr, size_t data_size)
{
	return valid_png(data_ptr, data_size) || valid_jpeg(data_ptr, data_size) ||
		valid_webp(data_ptr, data_size) || valid_dds(data_ptr, data_size) ||
		valid_atex(data_ptr, data_size) || valid_ateu(data_ptr, data_size) ||
		valid_atep(data_ptr, data_size);
}

void create_and_display_texture(const void* image_data_ptr, WindowData& window_data)
{
	if (window_data.image_data.texture_id == 0)
	{
		glGenTextures(1, &window_data.image_data.texture_id);
	}

	glBindTexture(GL_TEXTURE_2D, window_data.image_data.texture_id);

	// Set default texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	if (window_data.image_data.anet_image) // If image is compressed
	{
		GLenum internalFormat = 0;

		switch (window_data.image_data.format_data)
		{
			// S3TC (DXT) Formats
		case 0x41545844: internalFormat = GL_COMPRESSED_RED_RGTC1; break; // DXTA
		case 0x4C545844: internalFormat = GL_COMPRESSED_RED_RGTC1; break; // DXTL
		case 0x4E545844: internalFormat = GL_COMPRESSED_RG_RGTC2; break; // DXTN
		case 0x31545844: internalFormat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT; break; // DXT1
		case 0x32545844:
		case 0x33545844:
		case 0x34545844: internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT; break; // DXT2, DXT3, DXT4
		case 0x35545844: internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT; break; // DXT5

			// BC4/BC5 (ATI & Normal Map)
		case 0x55344342: internalFormat = GL_COMPRESSED_RED_RGTC1; break; // BC4 (ATI1)
		case 0x55354342: internalFormat = GL_COMPRESSED_RG_RGTC2; break; // BC5 (ATI2)

			// BC6H (HDR) & BC7
		case 0x48364342: internalFormat = GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB; break; // BC6H (HDR)
		case 0x58374342: internalFormat = GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB; break; // BC7

			// 3Dc (ATI compression)
		case 0x58434433: internalFormat = GL_COMPRESSED_RG_RGTC2; break; // 3Dc (BC5 variant)

		default:
			printf("Unsupported compressed format: %X\n", window_data.image_data.format_data);
			return;
		}

		glCompressedTexImage2D(GL_TEXTURE_2D, 0, internalFormat,
			window_data.image_data.image_height, window_data.image_data.image_width, 0,
			window_data.binary_data.decompressed_image.size(), image_data_ptr);

		// Display texture in ImGui
		ImGui::Image((ImTextureID)window_data.image_data.texture_id,
			ImVec2(window_data.image_data.image_height, window_data.image_data.image_width));
	}
	else // Uncompressed formats (RGB, RGBA, Grayscale)
	{
		GLenum format = GL_RGBA;
		GLenum internalFormat = GL_RGBA8; // Default to 8-bit RGBA

		switch (window_data.image_data.image_channel)
		{
		case 4: format = GL_RGBA; internalFormat = GL_RGBA8; break;  // RGBA 8-bit
		case 3: format = GL_RGB; internalFormat = GL_RGB8; break;    // RGB 8-bit
		case 2: format = GL_RG; internalFormat = GL_RG8; break;      // Grayscale + Alpha
		case 1: format = GL_RED; internalFormat = GL_R8; break;      // Grayscale
		default:
			printf("Unsupported image channel count: %d\n", window_data.image_data.image_channel);
			return;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat,
			window_data.image_data.image_width, window_data.image_data.image_height, 0,
			format, GL_UNSIGNED_BYTE, image_data_ptr);
		// Display texture in ImGui
		ImGui::Image((ImTextureID)window_data.image_data.texture_id,
			ImVec2(window_data.image_data.image_width, window_data.image_data.image_height));
	}


}



void display_image_png(Gw2Dat& data_gw2, WindowData& window_data)
{
	png_image image; // Structure to hold PNG image data
	memset(&image, 0, sizeof(image));
	image.version = PNG_IMAGE_VERSION;


	if (!png_image_begin_read_from_memory(&image, window_data.binary_data.decompressed_data.data(), window_data.binary_data.decompressed_data.size()))
	{
		ImGui::Text("Failed to begin reading PNG image.");
		return;
	}

	image.format = PNG_FORMAT_RGBA; // Request RGBA format

	std::vector<uint8_t> buffer(PNG_IMAGE_SIZE(image));
	if (!png_image_finish_read(&image, nullptr, buffer.data(), 0, nullptr))
	{
		ImGui::Text("Failed to read PNG image.");
		png_image_free(&image);
		return;
	}

	window_data.image_data.image_width = image.width;
	window_data.image_data.image_height = image.height;
	window_data.image_data.image_channel = 4; // RGBA
	window_data.image_data.anet_image = false;
	window_data.image_data.format_data = 0;

	ImGui::Text("Dimensions: %dx%d, Channels: %d", window_data.image_data.image_width, window_data.image_data.image_height, window_data.image_data.image_channel);
	create_and_display_texture(buffer.data(), window_data);

	png_image_free(&image); // Free image resources
}

void display_image_jpeg(Gw2Dat& data_gw2, WindowData& window_data)
{
	jpeg_decompress_struct cinfo;
	jpeg_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	jpeg_mem_src(&cinfo, window_data.binary_data.decompressed_data.data(), window_data.binary_data.decompressed_data.size());

	if (jpeg_read_header(&cinfo, TRUE) != JPEG_HEADER_OK)
	{
		ImGui::Text("Failed to read JPEG header.");
		jpeg_destroy_decompress(&cinfo);
		return;
	}

	jpeg_start_decompress(&cinfo);

	window_data.image_data.image_width = cinfo.output_width;
	window_data.image_data.image_height = cinfo.output_height;
	window_data.image_data.image_channel = cinfo.output_components;
	window_data.image_data.anet_image = false;
	window_data.image_data.format_data = 0;

	std::vector<uint8_t> buffer(window_data.image_data.image_width * window_data.image_data.image_height * window_data.image_data.image_channel);

	while (cinfo.output_scanline < cinfo.output_height)
	{
		uint8_t* row_pointer = buffer.data() + cinfo.output_scanline * window_data.image_data.image_width * window_data.image_data.image_channel;
		jpeg_read_scanlines(&cinfo, &row_pointer, 1);
	}

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	ImGui::Text("Dimensions: %dx%d, Channels: %d", window_data.image_data.image_width, window_data.image_data.image_height, window_data.image_data.image_channel);
	create_and_display_texture(buffer.data(), window_data);
}

void display_image_webp(Gw2Dat& data_gw2, WindowData& window_data)
{
	int width, height;
	uint8_t* webp_data = WebPDecodeRGBA(window_data.binary_data.decompressed_data.data(), window_data.binary_data.decompressed_data.size(), &width, &height);

	if (webp_data)
	{
		window_data.image_data.image_width = width;
		window_data.image_data.image_height = height;
		window_data.image_data.image_channel = 4; // WebP is RGBA
		window_data.image_data.anet_image = false;
		window_data.image_data.format_data = 0;
		create_and_display_texture(webp_data, window_data);
		WebPFree(webp_data);
	}
	else
	{
		ImGui::Text("Failed to decode WebP image.");
	}
}

void display_image_dds(Gw2Dat& data_gw2, WindowData& window_data) {
	DirectX::ScratchImage scratch_image;
	HRESULT hr = DirectX::LoadFromDDSMemory(window_data.binary_data.decompressed_data.data(), window_data.binary_data.decompressed_data.size(), DirectX::DDS_FLAGS_NONE, nullptr, scratch_image);

	if (SUCCEEDED(hr)) {
		const DirectX::Image* img = scratch_image.GetImage(0, 0, 0);
		window_data.image_data.image_width = img->width;
		window_data.image_data.image_height = img->height;
		window_data.image_data.image_channel = 4; // DDS textures are typically RGBA
		window_data.image_data.anet_image = false;
		window_data.image_data.format_data = 0;
		create_and_display_texture(img->pixels, window_data);
	}
	else {
		ImGui::Text("Failed to load DDS image.");
	}
}

void display_image_ATEX(Gw2Dat& data_gw2, WindowData& window_data) {


	window_data.image_data.image_width = window_data.anet_image.width;
	window_data.image_data.image_height = window_data.anet_image.height;
	window_data.image_data.image_channel = 0;
	window_data.image_data.anet_image = true;
	window_data.image_data.format_data = window_data.anet_image.format;

	create_and_display_texture(window_data.binary_data.decompressed_image.data(), window_data);

}

void render_image(Gw2Dat& data_gw2, WindowData& window_data)
{

	// Check file header for supported formats
	const uint8_t* data_ptr = reinterpret_cast<const uint8_t*>(window_data.binary_data.decompressed_data.data());
	size_t data_size = window_data.binary_data.decompressed_data.size();

	if (check_valid_image(data_ptr, data_size))
	{
		if (valid_png(data_ptr, data_size))
		{
			display_image_png(data_gw2, window_data);
		}
		else if (valid_jpeg(data_ptr, data_size)) {
			display_image_jpeg(data_gw2, window_data);
		}
		else if (valid_webp(data_ptr, data_size))
		{
			display_image_webp(data_gw2, window_data);
		}
		else if (valid_dds(data_ptr, data_size)) {
			display_image_dds(data_gw2, window_data);
		}
		else if (valid_atex(data_ptr, data_size))
		{
			display_image_ATEX(data_gw2, window_data);
		}
		else if (valid_ateu(data_ptr, data_size))
		{
			display_image_ATEX(data_gw2, window_data);
		}
		else if (valid_atep(data_ptr, data_size))
		{
			display_image_ATEX(data_gw2, window_data);
		}
		//else if (valid_ctex(data_ptr, data_size))
		//{
		//	display_image_ATEX(data_gw2, window_data);
		//}
		else
		{
			ImGui::Text("Unsupported image format or invalid file header.");
		}
	}
}

bool valid_bink2(const uint8_t* data_ptr, size_t data_size)
{
	// Check for "KB2" (basic Bink2 signature)
	if (data_size >= 4 &&
		data_ptr[0] == 'K' && data_ptr[1] == 'B' &&
		data_ptr[2] == '2')
	{
		return true; // "KB2"
	}
	return false;
}

void render_video(Gw2Dat& data_gw2, WindowData& window_data)
{
	// Check file header for supported formats
	const uint8_t* data_ptr = reinterpret_cast<const uint8_t*>(window_data.binary_data.decompressed_data.data());
	size_t data_size = window_data.binary_data.decompressed_data.size();
	if (valid_bink2(data_ptr, data_size))
	{

		if (!window_data.video_data.bink_handler)
		{
			window_data.video_data.bink_handler = BinkOpen(reinterpret_cast<const char*>(window_data.binary_data.decompressed_data.data()), BINKFROMMEMORY | BINKALPHA | BINKYCRCBNEW);
			if (!window_data.video_data.bink_handler)
			{
				std::cerr << "Failed to open Bink video." << std::endl;
				return;
			}

			// Create an OpenGL texture for displaying the image
			if (window_data.video_data.video_texture == 0)
			{
				glGenTextures(1, &window_data.video_data.video_texture);
			}
			glBindTexture(GL_TEXTURE_2D, window_data.video_data.video_texture);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}

		ImGui::Text("Loaded Bink: %d width, %d height", window_data.video_data.bink_handler->Width, window_data.video_data.bink_handler->Height);

		// Play/Pause button
		if (ImGui::Button(window_data.video_data.is_playing ? "Pause" : "Play"))
		{
			window_data.video_data.is_playing = !window_data.video_data.is_playing;
		}

		// Get the available space for the video in the Preview tab
		ImVec2 previewSize = ImGui::GetContentRegionAvail();
		float scaleX = previewSize.x / static_cast<float>(window_data.video_data.bink_handler->Width);
		float scaleY = previewSize.y / static_cast<float>(window_data.video_data.bink_handler->Height);
		// Manually choose the smaller scaling factor to maintain the aspect ratio
		float scale;
		if (scaleX < scaleY)
		{
			scale = scaleX;
		}
		else
		{
			scale = scaleY;
		}
		// Display the seek bar and duration
		if (window_data.video_data.bink_handler)
		{
			window_data.video_data.actual_framerate = static_cast<float>(window_data.video_data.bink_handler->FrameRate) / window_data.video_data.bink_handler->FrameRateDiv;
			window_data.video_data.current_time = static_cast<float>(window_data.video_data.bink_handler->FrameNum) / window_data.video_data.actual_framerate;
			window_data.video_data.total_time = static_cast<float>(window_data.video_data.bink_handler->Frames) / window_data.video_data.actual_framerate;

			ImGui::Text("Current: %.2f / %.2f Seconds", window_data.video_data.current_time, window_data.video_data.total_time);

			window_data.video_data.seek_time = window_data.video_data.current_time;
			if (ImGui::SliderFloat("Click to select a frame time", &window_data.video_data.seek_time, 0.0f, window_data.video_data.total_time))
			{
				int targetFrame = static_cast<int>(window_data.video_data.seek_time * window_data.video_data.actual_framerate);
				BinkGoto(window_data.video_data.bink_handler, targetFrame, 0);
			}
		}

		// Handle frame-by-frame or continuous playback
		if (window_data.video_data.is_playing && window_data.video_data.bink_handler)
		{
			// Wait until it's time for the next frame
			while (BinkWait(window_data.video_data.bink_handler))
			{
			}

			BinkDoFrame(window_data.video_data.bink_handler);
			void* rgbBuffer = malloc(static_cast<size_t>(window_data.video_data.bink_handler->FrameBuffers->YABufferWidth) * window_data.video_data.bink_handler->FrameBuffers->YABufferHeight * 4);
			if (!rgbBuffer)
			{
				throw std::runtime_error("Failed to decompress into buffer: Memory allocation failed.");
			}

			BinkCopyToBuffer(window_data.video_data.bink_handler, rgbBuffer, window_data.video_data.bink_handler->FrameBuffers->YABufferWidth * 4, window_data.video_data.bink_handler->FrameBuffers->YABufferHeight, 0, 0, BINKSURFACE32RGBA);

			glBindTexture(GL_TEXTURE_2D, window_data.video_data.video_texture);
			if (window_data.video_data.first_frame)
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, window_data.video_data.bink_handler->FrameBuffers->YABufferWidth, window_data.video_data.bink_handler->FrameBuffers->YABufferHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgbBuffer);
				window_data.video_data.first_frame = false;
			}
			else
			{
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, window_data.video_data.bink_handler->FrameBuffers->YABufferWidth, window_data.video_data.bink_handler->FrameBuffers->YABufferHeight, GL_RGBA, GL_UNSIGNED_BYTE, rgbBuffer);
			}

			ImVec2 videoSize = ImVec2(window_data.video_data.bink_handler->Width * scale, window_data.video_data.bink_handler->Height * scale);
			ImGui::Image((ImTextureID)window_data.video_data.video_texture, videoSize);

			BinkNextFrame(window_data.video_data.bink_handler);
			free(rgbBuffer);
		}
		else if (!window_data.video_data.is_playing && window_data.video_data.bink_handler)
		{
			// Display the paused frame
			void* rgbBuffer = malloc(static_cast<size_t>(window_data.video_data.bink_handler->FrameBuffers->YABufferWidth) * window_data.video_data.bink_handler->FrameBuffers->YABufferHeight * 4);
			if (!rgbBuffer)
			{
				throw std::runtime_error("Failed to decompress into buffer: Memory allocation failed.");
			}

			BinkCopyToBuffer(window_data.video_data.bink_handler, rgbBuffer, window_data.video_data.bink_handler->FrameBuffers->YABufferWidth * 4, window_data.video_data.bink_handler->FrameBuffers->YABufferHeight, 0, 0, BINKSURFACE32RGBA);

			glBindTexture(GL_TEXTURE_2D, window_data.video_data.video_texture);
			if (window_data.video_data.first_frame)
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, window_data.video_data.bink_handler->FrameBuffers->YABufferWidth, window_data.video_data.bink_handler->FrameBuffers->YABufferHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgbBuffer);
				window_data.video_data.first_frame = false;
			}
			else
			{
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, window_data.video_data.bink_handler->FrameBuffers->YABufferWidth, window_data.video_data.bink_handler->FrameBuffers->YABufferHeight, GL_RGBA, GL_UNSIGNED_BYTE, rgbBuffer);
			}

			ImVec2 videoSize = ImVec2(window_data.video_data.bink_handler->Width * scale, window_data.video_data.bink_handler->Height * scale);
			ImGui::Image((ImTextureID)window_data.video_data.video_texture, videoSize);

			free(rgbBuffer);
		}
	}
}

void cleanup_bink(WindowData& window_data)
{
	if (window_data.video_data.bink_handler)
	{
		BinkClose(window_data.video_data.bink_handler); // Close the Bink handler
		window_data.video_data.bink_handler = nullptr;
	}
}

void render_preview_tab(Gw2Dat& data_gw2, WindowData& window_data) {

	// Display preview data
	ImGui::Text("Preview Data:");

	if (window_data.dat_data.selected_base_id != window_data.dat_data.last_selected_video_decompressed)
	{
		cleanup_bink(window_data);
		window_data.video_data.is_playing = false;
		window_data.video_data.actual_framerate = 0.0f;
		window_data.video_data.current_time = 0.0f;
		window_data.video_data.total_time = 0.0f;
		window_data.video_data.first_frame = true;
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
	if (valid_atex(window_data.binary_data.decompressed_data.data(), window_data.binary_data.decompressed_data.size()) ||
		valid_ateu(window_data.binary_data.decompressed_data.data(), window_data.binary_data.decompressed_data.size()) ||
		valid_atep(window_data.binary_data.decompressed_data.data(), window_data.binary_data.decompressed_data.size()))
	{
		window_data.binary_data.decompressed_image = extract_decompressed_image(window_data.binary_data.decompressed_data, window_data.anet_image);
	}

	render_image(data_gw2, window_data);
	render_video(data_gw2, window_data);

}

void render_middle_panel(Gw2Dat& data_gw2, WindowData& window_data)
{
	ImGui::Begin("Extracted Data");

	if (ImGui::BeginTabBar("MFT Data Tabs"))
	{
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
			render_preview_tab(data_gw2, window_data);
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}

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
	ImGui::Text("Identifier: %.*s", HEADER_IDENTIFIER, selected_entry.identifier);
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

void run_window(Gw2Dat& data_gw2, WindowData& window_data) {
	while (!glfwWindowShouldClose(window_data.window)) {
		glfwPollEvents();

		if (glfwGetWindowAttrib(window_data.window, GLFW_ICONIFIED) != 0) {
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			continue;
		}

		// Start ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		ImGui::Begin("DockSpace", nullptr, window_flags);
		ImGui::PopStyleVar(2);

		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
			ImGuiID dockspace_id = ImGui::GetID("PanelDockspace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), window_data.dockspace_flags);
		}
		else {
			ImGui::Text("Docking is not enabled! Enable it in ImGuiIO.");
		}
		render_menu_bar();
		render_left_panel(data_gw2, window_data);
		render_middle_panel(data_gw2, window_data);
		render_right_panel(data_gw2, window_data);

		ImGui::Begin("Hello, ImGui!");
		ImGui::Text("This is an example of docking.");
		ImGui::End();

		ImGui::End(); // End the main window

		// Render
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window_data.window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(CLEAR_COLOR.x * CLEAR_COLOR.w, CLEAR_COLOR.y * CLEAR_COLOR.w, CLEAR_COLOR.z * CLEAR_COLOR.w, CLEAR_COLOR.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			GLFWwindow* backup_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_context);
		}

		glfwSwapBuffers(window_data.window);
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(window_data.window);
	glfwTerminate();
}

bool initialize_window(Gw2Dat& data_gw2, WindowData& window_data) {
	try {
		glfwSetErrorCallback([](int error, const char* description) {
			std::cerr << "GLFW Error " << error << ": " << description << '\n';
			});

		if (!glfwInit()) {
			throw std::runtime_error("Failed to initialize GLFW");
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

		window_data.window = glfwCreateWindow(1280, 720, window_data.window_title.c_str(), nullptr, nullptr);
		if (!window_data.window) {
			glfwTerminate();
			throw std::runtime_error("Failed to create GLFW window");
		}

		glfwMakeContextCurrent(window_data.window);
		glfwSwapInterval(1); // Enable vsync

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			throw std::runtime_error("Failed to initialize GLAD");
		}

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		ImGui::StyleColorsLight();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			ImGuiStyle& style = ImGui::GetStyle();
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		ImGui_ImplGlfw_InitForOpenGL(window_data.window, true);
		ImGui_ImplOpenGL3_Init("#version 130");

		run_window(data_gw2, window_data);
		return true;
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << '\n';
		glfwTerminate();
		return false;
	}
}



#endif // !APPLICATION_H
