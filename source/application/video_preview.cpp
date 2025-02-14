#include "application.h"



void render_video(Gw2Dat& data_gw2, WindowData& window_data, const uint8_t* data_ptr, size_t data_size)
{
	if (valid_bink2(data_ptr, data_size))
	{


		if (!window_data.video_data.bink_handler)
		{
			HRESULT hr = DirectSoundCreate(NULL, &window_data.video_data.lpDS, NULL);
			if (FAILED(hr)) {
				printf("Failed to initialize DirectSound.\n");
				return;
			}

			// Set cooperative level (needed for DirectSound)
			hr = window_data.video_data.lpDS->SetCooperativeLevel(GetDesktopWindow(), DSSCL_PRIORITY);
			if (FAILED(hr)) {
				printf("Failed to set DirectSound cooperative level.\n");
				window_data.video_data.lpDS->Release();
				return;
			}

			// Use DirectSound for Bink (MUST BE BEFORE BinkOpen)
			BinkSoundUseDirectSound(window_data.video_data.lpDS);

			window_data.video_data.bink_handler = BinkOpen(reinterpret_cast<const char*>(window_data.binary_data.decompressed_data.data()), BINKFROMMEMORY | BINKALPHA | BINKYCRCBNEW | BINKSNDTRACK);
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

		ImGui::Text("Loaded Bink: %d x %d @ %.2f FPS | Frames: %d/%d | Sound: %s",
			window_data.video_data.bink_handler->Width,
			window_data.video_data.bink_handler->Height,
			(float)window_data.video_data.bink_handler->FrameRate / window_data.video_data.bink_handler->FrameRateDiv,
			window_data.video_data.bink_handler->FrameNum,
			window_data.video_data.bink_handler->Frames,
			window_data.video_data.bink_handler->soundon ? "Enabled" : "Disabled"
		);


		// Volume control UI
		ImGui::Text("Volume Control");
		ImGui::SliderInt("Volume", &window_data.video_data.volume, 0, 100);


		if (ImGui::Button("Reset"))
		{
			if (window_data.video_data.bink_handler)
			{
				BinkGoto(window_data.video_data.bink_handler, 0, 0); // Seek to the first frame
				window_data.video_data.is_playing = false; // Start playing from the beginning
			}
		}

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
			int mapped_volume = (window_data.video_data.volume * 32768) / 100;
			BinkSetVolume(window_data.video_data.bink_handler, 0, mapped_volume);

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
		window_data.video_data.lpDS->Release();
	}
	window_data.video_data.is_playing = false;
	window_data.video_data.actual_framerate = 0.0f;
	window_data.video_data.current_time = 0.0f;
	window_data.video_data.total_time = 0.0f;
	window_data.video_data.first_frame = true;
}