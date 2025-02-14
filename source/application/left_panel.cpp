#include "application.h"

void render_left_panel(Gw2Dat& data_gw2, WindowData& window_data) {
	ImGui::Begin("MFT Data");

	ImGui::Text("Search Bar:");
	ImGui::InputInt("##SearchBar", &window_data.dat_data.search_number);
	ImGui::Separator();
	ImGui::Checkbox("Show By Base Id", &window_data.dat_data.show_by_base_id);
	ImGui::Separator();
	ImGui::Text("MFT Data List:");

	//ImGui::Text("MFT Id | Compressed | Uncompressed | File Id");


	if (window_data.dat_data.show_by_base_id)
	{
		ImGui::Text("Base Id | Compressed | Uncompressed | File Id");
	}
	else
	{
		ImGui::Text("File Id | Compressed | Uncompressed | Base Id");
	}


	ImVec2 child_size = ImVec2(0, 0); // Adjust height as needed
	ImGui::BeginChild("MFTList", child_size, true, ImGuiWindowFlags_HorizontalScrollbar);

	//if (window_data.dat_data.search_number > 0)
	//{

	//	if (window_data.dat_data.temp_number != window_data.dat_data.search_number)
	//	{
	//		window_data.dat_data.found_results = search_by_base_id(data_gw2, window_data.dat_data.search_number);
	//		window_data.dat_data.temp_number = window_data.dat_data.search_number;
	//	}
	//	const auto& mft_base_data = data_gw2.mft_base_id_data_list;
	//	const auto& mft_data = data_gw2.mft_data_list;

	//	size_t total_items = window_data.dat_data.found_results.size();
	//	// Use ImGuiListClipper for efficient rendering of large lists
	//	ImGuiListClipper clipper;
	//	clipper.Begin(static_cast<int>(total_items)); // Total number of items in the list

	//	while (clipper.Step())
	//	{
	//		for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
	//		{

	//			uint64_t base_id = window_data.dat_data.found_results[i];

	//			// Construct a unique label for each selectable item
	//			std::ostringstream oss;
	//			oss << std::setw(8) << std::left << base_id << " | "
	//				<< std::setw(8) << std::left << mft_data[base_id - 1].size << " | "
	//				<< std::setw(8) << std::left << mft_data[base_id - 1].uncompressed_size << " | [";

	//			// Loop through the file_id_ vector and add each element to the label string
	//			const auto& file_id = get_by_file_id(data_gw2, base_id);
	//			for (size_t j = 0; j < file_id.size(); ++j)
	//			{
	//				oss << std::setw(8) << std::left << file_id[j];
	//				if (j != file_id.size() - 1)
	//				{
	//					oss << ", ";
	//				}
	//			}
	//			oss << "]";

	//			std::string label = oss.str();

	//			// Display the selectable item
	//			if (ImGui::Selectable(label.c_str(), window_data.dat_data.selected_base_id == base_id))
	//			{
	//				// Update the selected index when clicked
	//				window_data.dat_data.selected_base_id = base_id;
	//			}

	//			// Handle copying with Ctrl + C
	//			if (ImGui::IsItemHovered() && ImGui::IsKeyPressed(ImGuiKey_C) && ImGui::GetIO().KeyCtrl)
	//			{
	//				// Copy the label text to the clipboard
	//				ImGui::SetClipboardText(label.c_str());
	//			}

	//		}
	//	}

	//	clipper.End();
	//}

	//if (window_data.dat_data.search_number == 0)
	//{
	//	// Retrieve the MFT base data
	//	const auto& mft_base_data = data_gw2.mft_base_id_data_list;
	//	const auto& mft_data = data_gw2.mft_data_list;

	//	size_t total_items = mft_data.size();

	//	// Ensure there are items to display
	//	if (total_items == 0)
	//	{
	//		ImGui::Text("No entries available.");
	//		return;
	//	}

	//	// Use ImGuiListClipper for efficient rendering of large lists
	//	ImGuiListClipper clipper;
	//	clipper.Begin(static_cast<int>(total_items)); // Total number of items in the list

	//	while (clipper.Step())
	//	{
	//		for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
	//		{

	//			// Construct a unique label for each selectable item
	//			std::ostringstream oss;
	//			oss << std::setw(8) << std::left << i << " | "
	//				<< std::setw(8) << std::left << mft_data[i].size << " | "
	//				<< std::setw(8) << std::left << mft_data[i].uncompressed_size << " | [";

	//			// Loop through the file_id vector and add each element to the label string
	//			const auto& file_id = mft_base_data[i].file_id;
	//			for (size_t j = 0; j < file_id.size(); ++j)
	//			{
	//				oss << std::setw(8) << std::left << file_id[j];
	//				if (j != file_id.size() - 1)
	//				{
	//					oss << ", ";
	//				}
	//			}

	//			oss << "]";

	//			std::string label = oss.str();



	//			// Display the selectable item
	//			if (ImGui::Selectable(label.c_str(), window_data.dat_data.selected_base_id == i))
	//			{
	//				// Update the selected index when clicked
	//				window_data.dat_data.selected_base_id = i;
	//			}

	//			// Handle copying with Ctrl + C
	//			if (ImGui::IsItemHovered() && ImGui::IsKeyPressed(ImGuiKey_C) && ImGui::GetIO().KeyCtrl)
	//			{
	//				// Copy the label text to the clipboard
	//				ImGui::SetClipboardText(label.c_str());
	//			}

	//		}
	//	}

	//	clipper.End();
	//}

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
			const auto& mft_data = data_gw2.mft_data_list;

			size_t total_items = window_data.dat_data.found_results.size();
			// Use ImGuiListClipper for efficient rendering of large lists
			ImGuiListClipper clipper;
			clipper.Begin(static_cast<int>(total_items)); // Total number of items in the list

			while (clipper.Step())
			{
				for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
				{

					uint64_t base_id = window_data.dat_data.found_results[i];
					if (base_id == 0) continue;  // Skip this specific item its gonna error in debug variant

					// Construct a unique label for each selectable item
					std::ostringstream oss;
					oss << std::setw(8) << std::left << base_id << " | "
						<< std::setw(8) << std::left << mft_data[base_id - 1].size << " | "
						<< std::setw(8) << std::left << mft_data[base_id - 1].uncompressed_size << " | [";

					// Loop through the file_id_ vector and add each element to the label string
					const auto& file_id = get_by_file_id(data_gw2, base_id);
					for (size_t j = 0; j < file_id.size(); ++j)
					{
						oss << std::setw(8) << std::left << file_id[j];
						if (j != file_id.size() - 1)
						{
							oss << ", ";
						}
					}
					oss << "]";

					std::string label = oss.str();

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
			const auto& mft_data = data_gw2.mft_data_list;

			size_t total_items = mft_base_data.size();

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
					if (mft_base_data[i].base_id == 0) continue;  // Skip this specific item its gonna error in debug variant

					// Construct a unique label for each selectable item
					std::ostringstream oss;
					oss << std::setw(8) << std::left << mft_base_data[i].base_id << " | "
						<< std::setw(8) << std::left << mft_data[mft_base_data[i].base_id - 1].size << " | "
						<< std::setw(8) << std::left << mft_data[mft_base_data[i].base_id - 1].uncompressed_size << " | [";

					// Loop through the file_id vector and add each element to the label string
					const auto& file_id = mft_base_data[i].file_id;
					for (size_t j = 0; j < file_id.size(); ++j)
					{
						oss << std::setw(8) << std::left << file_id[j];
						if (j != file_id.size() - 1)
						{
							oss << ", ";
						}
					}

					oss << "]";

					std::string label = oss.str();



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
			const auto& mft_data = data_gw2.mft_data_list;

			// Use ImGuiListClipper for efficient rendering of large lists
			ImGuiListClipper clipper;
			clipper.Begin(static_cast<int>(total_items)); // Total number of items in the list

			while (clipper.Step())
			{
				for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
				{
					uint64_t file_id = window_data.dat_data.found_results[i];
					uint64_t base_id = static_cast<uint64_t>(get_by_base_id(data_gw2, file_id));
					if (base_id == 0) continue;  // Skip this specific item its gonna error in debug variant
					// Construct a unique label for each selectable item
					std::ostringstream oss;
					oss << std::left << std::setw(8) << file_id
						<< " | " << std::setw(8) << mft_data[base_id - 1].size
						<< " | " << std::setw(8) << mft_data[base_id - 1].uncompressed_size
						<< " | " << std::setw(8) << base_id;
					std::string label = oss.str();

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
			const auto& mft_data = data_gw2.mft_data_list;

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
					if (mft_file_data[i].base_id == 0) continue;  // Skip this specific item its gonna error in debug variant

					// Construct a unique label for each selectable item
					std::ostringstream oss;
					oss << std::left << std::setw(8) << mft_file_data[i].file_id
						<< " | " << std::setw(8) << mft_data[mft_file_data[i].base_id - 1].size
						<< " | " << std::setw(8) << mft_data[mft_file_data[i].base_id - 1].uncompressed_size
						<< " | " << std::setw(8) << mft_file_data[i].base_id;
					std::string label = oss.str();
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