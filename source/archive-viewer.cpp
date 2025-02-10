// archive-viewer.cpp : Defines the entry point for the application.
//

#include "archive-viewer.h"
#include "application.h"


int main() {
	try {
		Gw2Dat gw2_dat_file{};
		WindowData window_data{};
		window_data.window = nullptr;
		window_data.window_title = "Archive Viewer";
		window_data.dockspace_flags = ImGuiDockNodeFlags_None;

		window_data.dat_data.search_number = 0;
		window_data.dat_data.temp_number = 0;
		window_data.dat_data.show_by_base_id = true;
		window_data.dat_data.selected_base_id = 16;
		window_data.dat_data.selected_file_id = 0;
		window_data.dat_data.last_selected_video_decompressed = 0;
		window_data.dat_data.last_selected_image_decompressed = 0;
		window_data.dat_data.status_message_timer = 5.0f;

		window_data.image_data.image_data = nullptr;
		window_data.image_data.texture_id = 0;
		window_data.image_data.image_width = 0;
		window_data.image_data.image_height = 0;
		window_data.image_data.image_channel = 0;
		window_data.image_data.anet_image = false;
		window_data.image_data.format_data = 0;


		window_data.video_data.bink_handler = nullptr;
		window_data.video_data.actual_framerate = 0.0f;
		window_data.video_data.current_time = 0.0f;
		window_data.video_data.total_time = 0.0f;
		window_data.video_data.seek_time = 0.0f;
		window_data.video_data.last_frametime = 0.0f;
		window_data.video_data.first_frame = true;
		window_data.video_data.is_playing = false;

		window_data.anet_image.format = 0;
		window_data.anet_image.identifier = 0;
		window_data.anet_image.width = 0;
		window_data.anet_image.height = 0;


		if (!load_dat_file(gw2_dat_file, "C:\\Users\\Ridwan Hidayatullah\\source\\repos\\archive-viewer\\Local.dat")) {
			std::cerr << "Failed to load DAT file." << std::endl;
			return 1; // Return non-zero to indicate an error
		}

		//if (!load_dat_file(gw2_dat_file, "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Guild Wars 2\\Gw2.dat")) {
		//	std::cerr << "Failed to load DAT file." << std::endl;
		//	return 1; // Return non-zero to indicate an error
		//}

		initialize_window(gw2_dat_file, window_data);

	}
	catch (const std::exception& e) {
		std::cerr << "Unexpected error: " << e.what() << std::endl;
		return 1; // Return non-zero to indicate an error
	}

	return 0; // Return 0 for successful execution
}
