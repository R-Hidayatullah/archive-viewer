#ifndef APPLICATION_H
#define APPLICATION_H

#include "parser.h"
#define GL_SILENCE_DEPRECATION
#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <chrono>
#include <thread>
#include <sstream>
#include <iomanip>

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
void flat_style();
void set_dark_theme();
void set_light_theme();
void set_classic_theme();
void set_flat_theme();
void render_menu_bar();
void render_left_panel(Gw2Dat& data_gw2, WindowData& window_data);
void render_compressed_tab(Gw2Dat& data_gw2, WindowData& window_data);
void render_decompressed_tab(Gw2Dat& data_gw2, WindowData& window_data);
bool valid_png(const uint8_t* data_ptr, size_t data_size);
bool valid_jpeg(const uint8_t* data_ptr, size_t data_size);
bool valid_webp(const uint8_t* data_ptr, size_t data_size);
bool valid_dds(const uint8_t* data_ptr, size_t data_size);
bool valid_atex(const uint8_t* data_ptr, size_t data_size);
bool valid_ateu(const uint8_t* data_ptr, size_t data_size);
bool valid_atep(const uint8_t* data_ptr, size_t data_size);
bool valid_ctex(const uint8_t* data_ptr, size_t data_size);
bool valid_bink2(const uint8_t* data_ptr, size_t data_size);
bool check_valid_image(const uint8_t* data_ptr, size_t data_size);
void check_image_channel(WindowData& window_data);
void create_and_display_texture(const void* image_data_ptr, WindowData& window_data);
void display_image_png(Gw2Dat& data_gw2, WindowData& window_data);
void display_image_jpeg(Gw2Dat& data_gw2, WindowData& window_data);
void display_image_webp(Gw2Dat& data_gw2, WindowData& window_data);
void display_image_dds(Gw2Dat& data_gw2, WindowData& window_data);
void display_image_ATEX(Gw2Dat& data_gw2, WindowData& window_data);
void render_image(Gw2Dat& data_gw2, WindowData& window_data);
void render_video(Gw2Dat& data_gw2, WindowData& window_data);
void render_model(Gw2Dat& data_gw2, WindowData& window_data);
void cleanup_bink(WindowData& window_data);
void render_preview_tab(Gw2Dat& data_gw2, WindowData& window_data);
void render_middle_panel(Gw2Dat& data_gw2, WindowData& window_data);
void display_header_info(Gw2Dat& data_gw2);
void display_status_message(WindowData& window_data);
void export_data_to_file(const std::string& filename, const std::vector<uint8_t>& data);
void display_selected_mft_data(Gw2Dat& data_gw2, WindowData& window_data);
void render_right_panel(Gw2Dat& data_gw2, WindowData& window_data);
void run_window(Gw2Dat& data_gw2, WindowData& window_data);
bool initialize_window(Gw2Dat& data_gw2, WindowData& window_data);
GLuint compileShader(GLenum type, const std::string& source);
GLuint createShaderProgram();
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void setupFramebuffer(int width, int height);
void render_model(Gw2Dat& data_gw2, WindowData& window_data);

#endif // !APPLICATION_H
