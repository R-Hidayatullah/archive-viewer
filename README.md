# Guild Wars 2 Archive Viewer

A lightweight tool for exploring and extracting data from Guild Wars 2 archives.

> ⚠️ **Note:** Must be compiled in **Release mode** to avoid ImGui clipper assertion errors (`clipper->ItemsHeight > 0.0f`).

---

## Features

- Load **any image format** supported by the application.
- Play **Bink2 video** files.
- Search archives by **Base ID** or **File ID**.
- Export **compressed** and **decompressed** data.
- Built-in **hex viewer** for inspecting file contents.

---

## Requirements

- C++11 compatible compiler
- OpenGL 3.3 or higher
- GLFW
- ImGui docking branch
- GLAD or GLEW (for OpenGL function loading)
- SDL2 (for video/audio playback if needed)

---

## Build Instructions

1. Clone the repository:

```bash
git clone https://github.com/R-Hidayatullah/archive-viewer.git
cd archive-viewer
````

2. Create a build directory and configure with CMake:

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

> Make sure to run in **Release mode**, otherwise ImGui may throw clipper assertion errors.

---

## Usage

* Open an archive file via **File → Open**.
* Browse files using the **Archive Browser** panel.
* Preview files in the **File Viewer** (images, hex, video playback).
* Inspect metadata and export data from the **File Info** panel.
* Use the search bar to locate files by **Base ID** or **File ID**.


---

## Notes

* Supports real-time search and preview of large archives.
* Hex viewer allows inspection of both compressed and decompressed data.
* For best performance, always run in **Release mode**.
