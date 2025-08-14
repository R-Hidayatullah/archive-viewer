#include "application.h"

static RenderData temp_render_data;

GLuint compileShader(GLenum type, const std::string& source) {
	GLuint shader = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(shader, 1, &src, nullptr);
	glCompileShader(shader);

	// Check for errors
	int success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader, 512, nullptr, infoLog);
		printf("Shader Compilation Error: %s\n", infoLog);
	}

	return shader;
}

GLuint createShaderProgram() {
	GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
	GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

	GLuint program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	// Check for linking errors
	int success;
	char infoLog[512];
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 512, nullptr, infoLog);
		printf("Shader Program Linking Error: %s\n", infoLog);
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return program;
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (temp_render_data.preview_tab_active) {

		if (temp_render_data.is_dragging) {
			float dx = xpos - temp_render_data.last_x;
			float dy = ypos - temp_render_data.last_y;
			temp_render_data.rotation_x += dy * 0.5f;
			temp_render_data.rotation_y += dx * 0.5f;
		}
		temp_render_data.last_x = xpos;
		temp_render_data.last_y = ypos;
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	if (temp_render_data.preview_tab_active) {
		temp_render_data.zoom -= (float)yoffset * 0.5f;
		if (temp_render_data.zoom < 1.0f) temp_render_data.zoom = 1.0f;
		if (temp_render_data.zoom > 10.0f) temp_render_data.zoom = 10.0f;
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (temp_render_data.preview_tab_active) {
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			temp_render_data.is_dragging = (action == GLFW_PRESS);
			glfwGetCursorPos(window, &temp_render_data.last_x, &temp_render_data.last_y);
		}
	}
}

void setupFramebuffer(int width, int height) {
	if (temp_render_data.FBO == 0) {
		glGenFramebuffers(1, &temp_render_data.FBO);
		glGenTextures(1, &temp_render_data.texture_color_buffer);
		glGenRenderbuffers(1, &temp_render_data.RBO);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, temp_render_data.FBO);

	// Setup color attachment (texture)
	glBindTexture(GL_TEXTURE_2D, temp_render_data.texture_color_buffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, temp_render_data.texture_color_buffer, 0);

	// Setup depth and stencil buffer
	glBindRenderbuffer(GL_RENDERBUFFER, temp_render_data.RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, temp_render_data.RBO);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void render_model(Gw2Dat& data_gw2, WindowData& window_data, const uint8_t* data_ptr, size_t data_size) {

	temp_render_data.preview_tab_active = window_data.preview_tab_active;

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
		std::cout << "Parsing PF data!\n";


	}

	if (window_data.chunk_data_model.found)
	{
		MemReader reader(window_data.binary_data.decompressed_data);

		HeaderDat header;
		if (!readStruct(reader, header)) return;

		// Only need the first GEOM chunk

		while (reader.size > 0) {
			ChunkData chunk;
			if (!readChunk(reader, chunk)) break;

			// Check if this chunk is GEOM
			if (std::memcmp(chunk.header.magic, "GEOM", 4) == 0) {
				window_data.chunk_data_model.geom_chunk = chunk; // save the chunk
				window_data.chunk_data_model.found = true;
				break;             // stop after first GEOM chunk
			}
		}

		if (!window_data.chunk_data_model.found) {
			std::cerr << "No GEOM chunk found!\n";
			return;
		}

		if (window_data.chunk_data_model.found) {
			ChunkData& geom = window_data.chunk_data_model.geom_chunk;

			// Print chunk header info
			std::cout << "Found GEOM chunk!\n";
			std::cout << "Magic: "
				<< geom.header.magic[0] << geom.header.magic[1]
				<< geom.header.magic[2] << geom.header.magic[3] << "\n";
			std::cout << "Chunk size: " << geom.header.chunkSize << "\n";
			std::cout << "Version: " << geom.header.version << "\n";
			std::cout << "Header size: " << geom.header.headerSize << "\n";
			std::cout << "Offset table offset: " << geom.header.offsetToOffsetTable << "\n";

			// Print sizes of contained buffers
			std::cout << "Data size: " << geom.data.size() << "\n";
			std::cout << "Offsets count: " << geom.offsets.size() << "\n";
			std::cout << "Unknown size: " << geom.unknown.size() << "\n";

			// Optional: print first few bytes of data as hex
			std::cout << "First 16 bytes of data: ";
			for (size_t i = 0; i < geom.data.size() && i < 16; ++i) {
				printf("%02X ", geom.data[i]);
			}
			std::cout << "\n";
		}

	}

	// Get panel size
	ImVec2 previewSize = ImGui::GetContentRegionAvail();
	int width = static_cast<int>(previewSize.x);
	int height = static_cast<int>(previewSize.y);

	// Ensure framebuffer is set up with correct size
	setupFramebuffer(width, height);

	// Bind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, temp_render_data.FBO);
	glViewport(0, 0, width, height);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (temp_render_data.shader_program == 0) {
		temp_render_data.shader_program = createShaderProgram();
	}

	if (temp_render_data.VAO == 0) {
		float vertices[] = {
			// Positions         // Colors
			-1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, // 0
			 1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // 1
			 1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 1.0f, // 2
			-1.0f,  1.0f, -1.0f, 1.0f, 1.0f, 0.0f, // 3
			-1.0f, -1.0f,  1.0f, 1.0f, 0.0f, 1.0f, // 4
			 1.0f, -1.0f,  1.0f, 0.0f, 1.0f, 1.0f, // 5
			 1.0f,  1.0f,  1.0f, 1.0f, 1.0f, 1.0f, // 6
			-1.0f,  1.0f,  1.0f, 0.0f, 0.0f, 0.0f  // 7
		};

		unsigned int indices[] = {
			0, 1, 2, 2, 3, 0, // Front
			1, 5, 6, 6, 2, 1, // Right
			5, 4, 7, 7, 6, 5, // Back
			4, 0, 3, 3, 7, 4, // Left
			3, 2, 6, 6, 7, 3, // Top
			4, 5, 1, 1, 0, 4  // Bottom
		};

		glGenVertexArrays(1, &temp_render_data.VAO);
		glGenBuffers(1, &temp_render_data.VBO);
		glGenBuffers(1, &temp_render_data.EBO);

		glBindVertexArray(temp_render_data.VAO);
		glBindBuffer(GL_ARRAY_BUFFER, temp_render_data.VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, temp_render_data.EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}

	// Setup camera matrices
	float aspectRatio = (float)width / (float)height;
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
	glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, temp_render_data.zoom), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(temp_render_data.rotation_x), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(temp_render_data.rotation_y), glm::vec3(0.0f, 1.0f, 0.0f));

	glUseProgram(temp_render_data.shader_program);

	GLint modelLoc = glGetUniformLocation(temp_render_data.shader_program, "model");
	GLint viewLoc = glGetUniformLocation(temp_render_data.shader_program, "view");
	GLint projLoc = glGetUniformLocation(temp_render_data.shader_program, "projection");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);

	// Render cube
	glBindVertexArray(temp_render_data.VAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind framebuffer

	// Draw ImGui Panel
	ImGui::Image((ImTextureID)temp_render_data.texture_color_buffer, previewSize, ImVec2(0, 1), ImVec2(1, 0));


}



