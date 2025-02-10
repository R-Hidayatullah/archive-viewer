#include "application.h"

void create_and_display_texture(const void* image_data_ptr, WindowData& window_data)
{
	if (window_data.image_data.texture_id == 0)
	{
		glGenTextures(1, &window_data.image_data.texture_id);
	}

	glBindTexture(GL_TEXTURE_2D, window_data.image_data.texture_id);

	// Set default texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	ImVec2 previewSize = ImGui::GetContentRegionAvail(); // Get available space

	int texWidth = window_data.image_data.image_width;
	int texHeight = window_data.image_data.image_height;

	if (window_data.image_data.anet_image) // If image is compressed
	{
		GLenum internalFormat = 0;

		switch (window_data.image_data.format_data)
		{
			// S3TC (DXT) Formats
		case 0x41545844: internalFormat = GL_COMPRESSED_RED_RGTC1; break; // DXTA
		case 0x4C545844: internalFormat = GL_COMPRESSED_RED_RGTC1; break; // DXTL
		case 0x4E545844: internalFormat = GL_COMPRESSED_RG_RGTC2; break; // DXTN
		case 0x31545844: internalFormat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT; break; // DXT1
		case 0x32545844:
		case 0x33545844:
		case 0x34545844: internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT; break; // DXT2, DXT3, DXT4
		case 0x35545844: internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT; break; // DXT5

			// BC4/BC5 (ATI & Normal Map)
		case 0x55344342: internalFormat = GL_COMPRESSED_RED_RGTC1; break; // BC4 (ATI1)
		case 0x55354342: internalFormat = GL_COMPRESSED_RG_RGTC2; break; // BC5 (ATI2)

			// BC6H (HDR) & BC7
		case 0x48364342: internalFormat = GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB; break; // BC6H (HDR)
		case 0x58374342: internalFormat = GL_COMPRESSED_RGBA_BPTC_UNORM_ARB; break; // BC7

			// 3Dc (ATI compression)
		case 0x58434433: internalFormat = GL_COMPRESSED_RG_RGTC2; break; // 3Dc (BC5 variant)

		default:
			printf("Unsupported compressed format: %X\n", window_data.image_data.format_data);
			return;
		}

		// Swap width and height for compressed images
		std::swap(texWidth, texHeight);

		glCompressedTexImage2D(GL_TEXTURE_2D, 0, internalFormat,
			texWidth, texHeight, 0,
			window_data.binary_data.decompressed_image.size(), image_data_ptr);
	}
	else // Uncompressed formats (RGB, RGBA, Grayscale)
	{
		GLenum format = GL_RGBA;
		GLenum internalFormat = GL_RGBA8; // Default to 8-bit RGBA

		switch (window_data.image_data.image_channel)
		{
		case 4: format = GL_RGBA; internalFormat = GL_RGBA8; break;  // RGBA 8-bit
		case 3: format = GL_RGB; internalFormat = GL_RGB8; break;    // RGB 8-bit
		case 2: format = GL_RG; internalFormat = GL_RG8; break;      // Grayscale + Alpha
		case 1: format = GL_RED; internalFormat = GL_R8; break;      // Grayscale
		default:
			printf("Unsupported image channel count: %d\n", window_data.image_data.image_channel);
			return;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat,
			texWidth, texHeight, 0,
			format, GL_UNSIGNED_BYTE, image_data_ptr);
	}

	// Maintain Aspect Ratio
	float imageAspect = (float)texWidth / (float)texHeight;
	float displayWidth = previewSize.x;
	float displayHeight = displayWidth / imageAspect;

	if (displayHeight > previewSize.y) // If height exceeds, adjust width
	{
		displayHeight = previewSize.y;
		displayWidth = displayHeight * imageAspect;
	}

	// Display texture in ImGui
	ImGui::Image((ImTextureID)window_data.image_data.texture_id, ImVec2(displayWidth, displayHeight));
}

void display_image_png(Gw2Dat& data_gw2, WindowData& window_data)
{
	png_image image; // Structure to hold PNG image data
	memset(&image, 0, sizeof(image));
	image.version = PNG_IMAGE_VERSION;


	if (!png_image_begin_read_from_memory(&image, window_data.binary_data.decompressed_data.data(), window_data.binary_data.decompressed_data.size()))
	{
		ImGui::Text("Failed to begin reading PNG image.");
		return;
	}

	image.format = PNG_FORMAT_RGBA; // Request RGBA format

	std::vector<uint8_t> buffer(PNG_IMAGE_SIZE(image));
	if (!png_image_finish_read(&image, nullptr, buffer.data(), 0, nullptr))
	{
		ImGui::Text("Failed to read PNG image.");
		png_image_free(&image);
		return;
	}

	window_data.image_data.image_width = image.width;
	window_data.image_data.image_height = image.height;
	window_data.image_data.image_channel = 4; // RGBA
	window_data.image_data.anet_image = false;
	window_data.image_data.format_data = 0;

	ImGui::Text("Dimensions: %dx%d, Channels: %d", window_data.image_data.image_width, window_data.image_data.image_height, window_data.image_data.image_channel);
	create_and_display_texture(buffer.data(), window_data);

	png_image_free(&image); // Free image resources
}

void display_image_jpeg(Gw2Dat& data_gw2, WindowData& window_data)
{
	jpeg_decompress_struct cinfo;
	jpeg_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	jpeg_mem_src(&cinfo, window_data.binary_data.decompressed_data.data(), window_data.binary_data.decompressed_data.size());

	if (jpeg_read_header(&cinfo, TRUE) != JPEG_HEADER_OK)
	{
		ImGui::Text("Failed to read JPEG header.");
		jpeg_destroy_decompress(&cinfo);
		return;
	}

	jpeg_start_decompress(&cinfo);

	window_data.image_data.image_width = cinfo.output_width;
	window_data.image_data.image_height = cinfo.output_height;
	window_data.image_data.image_channel = cinfo.output_components;
	window_data.image_data.anet_image = false;
	window_data.image_data.format_data = 0;

	std::vector<uint8_t> buffer(window_data.image_data.image_width * window_data.image_data.image_height * window_data.image_data.image_channel);

	while (cinfo.output_scanline < cinfo.output_height)
	{
		uint8_t* row_pointer = buffer.data() + cinfo.output_scanline * window_data.image_data.image_width * window_data.image_data.image_channel;
		jpeg_read_scanlines(&cinfo, &row_pointer, 1);
	}

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	ImGui::Text("Dimensions: %dx%d, Channels: %d", window_data.image_data.image_width, window_data.image_data.image_height, window_data.image_data.image_channel);
	create_and_display_texture(buffer.data(), window_data);
}

void display_image_webp(Gw2Dat& data_gw2, WindowData& window_data)
{
	int width, height;
	uint8_t* webp_data = WebPDecodeRGBA(window_data.binary_data.decompressed_data.data(), window_data.binary_data.decompressed_data.size(), &width, &height);

	if (webp_data)
	{
		window_data.image_data.image_width = width;
		window_data.image_data.image_height = height;
		window_data.image_data.image_channel = 4; // WebP is RGBA
		window_data.image_data.anet_image = false;
		window_data.image_data.format_data = 0;
		ImGui::Text("Dimensions: %dx%d, Channels: %d", window_data.image_data.image_width, window_data.image_data.image_height, window_data.image_data.image_channel);
		create_and_display_texture(webp_data, window_data);
		WebPFree(webp_data);
	}
	else
	{
		ImGui::Text("Failed to decode WebP image.");
	}
}

void display_image_dds(Gw2Dat& data_gw2, WindowData& window_data) {
	DirectX::ScratchImage scratch_image;
	HRESULT hr = DirectX::LoadFromDDSMemory(window_data.binary_data.decompressed_data.data(), window_data.binary_data.decompressed_data.size(), DirectX::DDS_FLAGS_NONE, nullptr, scratch_image);

	if (SUCCEEDED(hr)) {
		const DirectX::Image* img = scratch_image.GetImage(0, 0, 0);
		window_data.image_data.image_width = img->width;
		window_data.image_data.image_height = img->height;
		window_data.image_data.image_channel = 4; // DDS textures are typically RGBA
		window_data.image_data.anet_image = false;
		window_data.image_data.format_data = 0;
		ImGui::Text("Dimensions: %dx%d, Channels: %d", window_data.image_data.image_width, window_data.image_data.image_height, window_data.image_data.image_channel);
		create_and_display_texture(img->pixels, window_data);
	}
	else {
		ImGui::Text("Failed to load DDS image.");
	}
}

void display_image_ATEX(Gw2Dat& data_gw2, WindowData& window_data) {


	window_data.image_data.image_width = window_data.anet_image.width;
	window_data.image_data.image_height = window_data.anet_image.height;
	window_data.image_data.anet_image = true;
	window_data.image_data.format_data = window_data.anet_image.format;
	window_data.image_data.image_channel = 0;
	check_image_channel(window_data);
	ImGui::Text("Dimensions: %dx%d, Channels: %d", window_data.image_data.image_height, window_data.image_data.image_width, window_data.image_data.image_channel);
	create_and_display_texture(window_data.binary_data.decompressed_image.data(), window_data);

}

void render_image(Gw2Dat& data_gw2, WindowData& window_data)
{

	// Check file header for supported formats
	const uint8_t* data_ptr = reinterpret_cast<const uint8_t*>(window_data.binary_data.decompressed_data.data());
	size_t data_size = window_data.binary_data.decompressed_data.size();

	if (check_valid_image(data_ptr, data_size))
	{
		if (valid_png(data_ptr, data_size))
		{
			display_image_png(data_gw2, window_data);
		}
		else if (valid_jpeg(data_ptr, data_size)) {
			display_image_jpeg(data_gw2, window_data);
		}
		else if (valid_webp(data_ptr, data_size))
		{
			display_image_webp(data_gw2, window_data);
		}
		else if (valid_dds(data_ptr, data_size)) {
			display_image_dds(data_gw2, window_data);
		}
		else if (valid_atex(data_ptr, data_size))
		{
			display_image_ATEX(data_gw2, window_data);
		}
		else if (valid_ateu(data_ptr, data_size))
		{
			display_image_ATEX(data_gw2, window_data);
		}
		else if (valid_atep(data_ptr, data_size))
		{
			display_image_ATEX(data_gw2, window_data);
		}
		//else if (valid_ctex(data_ptr, data_size))
		//{
		//	display_image_ATEX(data_gw2, window_data);
		//}
		else
		{
			ImGui::Text("Unsupported image format or invalid file header.");
		}
	}
}