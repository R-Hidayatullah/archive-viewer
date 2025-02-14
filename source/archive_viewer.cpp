// archive-viewer.cpp : Defines the entry point for the application.
//

#include "archive_viewer.h"
#include "application.h"

int main() {
	try {
		Gw2Dat gw2_dat_file{};
		WindowData window_data{};
		window_data.window_title = "Archive Viewer";


		//if (!load_dat_file(gw2_dat_file, "C:\\Users\\Ridwan Hidayatullah\\source\\repos\\archive-viewer\\Local.dat")) {
		//	std::cerr << "Failed to load DAT file." << std::endl;
		//	return 1; // Return non-zero to indicate an error
		//}

		if (!load_dat_file(gw2_dat_file, "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Guild Wars 2\\Gw2.dat")) {
			std::cerr << "Failed to load DAT file." << std::endl;
			return 1; // Return non-zero to indicate an error
		}

		initialize_window(gw2_dat_file, window_data);

	}
	catch (const std::exception& e) {
		std::cerr << "Unexpected error: " << e.what() << std::endl;
		return 1; // Return non-zero to indicate an error
	}

	return 0; // Return 0 for successful execution
}
