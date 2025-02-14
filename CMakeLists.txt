# CMakeList.txt : CMake project for archive-viewer, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("archive-viewer")

set(CMAKE_TOOLCHAIN_FILE "C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\VC\\vcpkg\\scripts\\buildsystems\\vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)
include_directories(external/glad33compat/include)
include_directories(external/imgui-docking)
include_directories(external/imgui-docking/backends)
include_directories(external/bink2/include)

find_package(directxtex CONFIG REQUIRED)
find_package(JPEG REQUIRED)
find_package(Ogg CONFIG REQUIRED)
find_package(Vorbis CONFIG REQUIRED)
find_package(PNG REQUIRED)
find_package(WebP CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(mpg123 CONFIG REQUIRED)

add_subdirectory(external/glad33compat)


set(GW2DAT_SOURCES
    source/gw2dattools/huffmanTreeUtils.cpp
    source/gw2dattools/inflateDatFileBuffer.cpp
    source/gw2dattools/inflateTextureFileBuffer.cpp
)

set(IMGUI_SOURCES
    external/imgui-docking/imgui.cpp
    external/imgui-docking/imgui_demo.cpp
    external/imgui-docking/imgui_draw.cpp
    external/imgui-docking/imgui_widgets.cpp
    external/imgui-docking/imgui_tables.cpp
    external/imgui-docking/backends/imgui_impl_opengl3.cpp
    external/imgui-docking/backends/imgui_impl_glfw.cpp
)

link_directories(
    external/bink2/lib
)

#add_compile_options(-Wall)

set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")

# Add source to this project's executable.
add_executable (${PROJECT_NAME}
    ${IMGUI_SOURCES}
    ${GW2DAT_SOURCES}
    "source/archive_viewer.cpp" 
    "source/parser/print_info.cpp" 
    "source/parser/parse_file.cpp" 
    "source/parser/extract_data.cpp" 
    "source/parser/search_file.cpp" 
    "source/application/image_preview.cpp" 
    "source/application/main_panel.cpp" 
    "source/application/left_panel.cpp" 
    "source/application/right_panel.cpp" 
    "source/application/middle_panel.cpp" 
    "source/application/video_preview.cpp" 
    "source/application/binary_preview.cpp" 
    "source/application/extension_checker.cpp" 
    "source/application/model_preview.cpp" )

#if (CMAKE_VERSION VERSION_GREATER 3.12)
#  set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
#endif()

# TODO: Add tests and install targets if needed.

target_include_directories(${PROJECT_NAME} PRIVATE JPEG::JPEG)

# Link libraries to your executable
target_link_libraries(${PROJECT_NAME} 
    Microsoft::DirectXTex
    glfw
    opengl32
    glad
    bink2w64
    dsound 
    dxguid
    glm::glm
    Ogg::ogg
    PNG::PNG
    JPEG::JPEG
    WebP::webp 
    WebP::webpdecoder 
    WebP::webpdemux
    MPG123::libmpg123
)