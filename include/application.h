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

#include <dsound.h>
#include "bink_2_7d.h"

struct BinaryData {
	std::vector<uint8_t> decompressed_data;
	std::vector<uint8_t> decompressed_image;
	std::vector<uint8_t> compressed_data;
};

struct ImageData {
	unsigned char* image_data = nullptr;
	GLuint texture_id = 0;
	int image_width = 0;
	int image_height = 0;
	int image_channel = 0;
	bool anet_image = false;
	uint32_t format_data = 0;
};

struct VideoData {
	HBINK bink_handler = nullptr;
	GLuint video_texture = 0;
	bool is_playing = false;
	bool first_frame = true;
	float current_time = 0.0f;
	float total_time = 0.0f;
	float seek_time = 0.0f;
	float last_frametime = 0.0f;
	float actual_framerate = 0.0f;
	LPDIRECTSOUND direct_sound = NULL;
	int volume = 100;
};

struct RenderData {
	float rotation_x = 0.0f;
	float rotation_y = 0.0f;
	float zoom = 10.0f;
	bool is_dragging = false;
	double last_x = 0.0;
	double last_y = 0.0;
	GLuint VAO = 0;
	GLuint VBO = 0;
	GLuint EBO = 0;
	GLuint shader_program = 0;
	GLuint FBO = 0;
	GLuint texture_color_buffer = 0;
	GLuint RBO = 0;
	bool preview_tab_active = false;
};

struct DatData {
	int search_number = 0;
	int temp_number = 0;
	uint64_t selected_base_id = 16;
	uint64_t selected_file_id = 0;
	bool show_by_base_id = true;
	std::vector<uint32_t> found_results = {};
	float status_message_timer = 5.0f;
	std::string status_message = "";
	uint64_t last_selected_item_compressed = 0;
	uint64_t last_selected_item_decompressed = 0;
	uint64_t last_selected_video_decompressed = 0;
	uint64_t last_selected_image_decompressed = 0;
};

struct WindowData {
	GLFWwindow* window = nullptr;
	std::string window_title = "Default Window";
	ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
	DatData dat_data = {};
	VideoData video_data = {};
	ImageData image_data = {};
	BinaryData binary_data = {};
	bool preview_tab_active = false;
	gw2dt::compression::AnetImage anet_image = {};
};

const std::string vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 vertexColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vertexColor = aColor;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

const std::string fragmentShaderSource = R"(
#version 330 core
in vec3 vertexColor;
out vec4 FragColor;

void main()
{
    FragColor = vec4(vertexColor, 1.0);
}
)";

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
bool valid_pf_modl(const uint8_t* data_ptr, size_t data_size);
bool valid_bink2(const uint8_t* data_ptr, size_t data_size);
bool check_valid_image(const uint8_t* data_ptr, size_t data_size);
void check_image_channel(WindowData& window_data);
void create_and_display_texture(const void* image_data_ptr, WindowData& window_data);
void display_image_png(WindowData& window_data);
void display_image_jpeg(WindowData& window_data);
void display_image_webp(WindowData& window_data);
void display_image_dds(WindowData& window_data);
void display_image_atex(WindowData& window_data);
void render_image(WindowData& window_data, const uint8_t* data_ptr, size_t data_size);
void render_video(WindowData& window_data, const uint8_t* data_ptr, size_t data_size);
void render_model(Gw2Dat& data_gw2, WindowData& window_data, const uint8_t* data_ptr, size_t data_size);
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
void render_model(Gw2Dat& data_gw2, WindowData& window_data, const uint8_t* data_ptr, size_t data_size);

#endif // !APPLICATION_H
