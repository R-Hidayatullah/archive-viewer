#include "application.h"

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


static float rotationX = 0.0f;
static float rotationY = 0.0f;
static float zoom = 5.0f;
static bool isDragging = false;
static double lastX, lastY;
static GLuint VAO = 0, VBO = 0, EBO = 0;
static GLuint shaderProgram = 0;
static GLuint FBO = 0, textureColorbuffer = 0, RBO = 0;
bool preview_tab_active = false; // Global or pass it via WindowData


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
    if (preview_tab_active) {

        if (isDragging) {
            float dx = xpos - lastX;
            float dy = ypos - lastY;
            rotationX += dy * 0.5f;
            rotationY += dx * 0.5f;
        }
        lastX = xpos;
        lastY = ypos;
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (preview_tab_active) {
        zoom -= yoffset * 0.5f;
        if (zoom < 1.0f) zoom = 1.0f;
        if (zoom > 10.0f) zoom = 10.0f;
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (preview_tab_active) {
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            isDragging = (action == GLFW_PRESS);
            glfwGetCursorPos(window, &lastX, &lastY);
        }
    }
}

void setupFramebuffer(int width, int height) {
    if (FBO == 0) {
        glGenFramebuffers(1, &FBO);
        glGenTextures(1, &textureColorbuffer);
        glGenRenderbuffers(1, &RBO);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    // Setup color attachment (texture)
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

    // Setup depth and stencil buffer
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void render_model(Gw2Dat& data_gw2, WindowData& window_data) {
    // Get panel size
    ImVec2 previewSize = ImGui::GetContentRegionAvail();
    int width = static_cast<int>(previewSize.x);
    int height = static_cast<int>(previewSize.y);

    // Ensure framebuffer is set up with correct size
    setupFramebuffer(width, height);

    // Bind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (shaderProgram == 0) {
        shaderProgram = createShaderProgram();
    }

    if (VAO == 0) {
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

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }

    // Setup camera matrices
    float aspectRatio = (float)width / (float)height;
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, zoom), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(rotationX), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotationY), glm::vec3(0.0f, 1.0f, 0.0f));

    glUseProgram(shaderProgram);

    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);

    // Render cube
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind framebuffer

    // Draw ImGui Panel
    ImGui::Image((ImTextureID)textureColorbuffer, previewSize, ImVec2(0, 1), ImVec2(1, 0));
}


void render_middle_panel(Gw2Dat& data_gw2, WindowData& window_data)
{
    ImGui::Begin("Extracted Data");

    if (ImGui::BeginTabBar("MFT Data Tabs"))
    {
        preview_tab_active = false;  // Reset flag before checking active tab

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

            if (!preview_tab_active) {
                preview_tab_active = true;
            }
            }
            render_preview_tab(data_gw2, window_data);
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
}

