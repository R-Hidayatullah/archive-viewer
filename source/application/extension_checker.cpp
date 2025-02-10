#include "application.h"

bool valid_png(const uint8_t* data_ptr, size_t data_size)
{
	return (data_size >= 8 &&
		data_ptr[0] == 0x89 && data_ptr[1] == 0x50 &&
		data_ptr[2] == 0x4E && data_ptr[3] == 0x47 &&
		data_ptr[4] == 0x0D && data_ptr[5] == 0x0A &&
		data_ptr[6] == 0x1A && data_ptr[7] == 0x0A);
}

bool valid_jpeg(const uint8_t* data_ptr, size_t data_size)
{
	return (data_size >= 3 &&
		data_ptr[0] == 0xFF && data_ptr[1] == 0xD8 &&
		data_ptr[2] == 0xFF);
}

bool valid_webp(const uint8_t* data_ptr, size_t data_size)
{
	return (data_size >= 12 &&
		data_ptr[0] == 0x52 && data_ptr[1] == 0x49 &&
		data_ptr[2] == 0x46 && data_ptr[3] == 0x46); // "RIFF"
}

bool valid_dds(const uint8_t* data_ptr, size_t data_size)
{
	return (data_size >= 4 &&
		data_ptr[0] == 'D' && data_ptr[1] == 'D' &&
		data_ptr[2] == 'S' && data_ptr[3] == ' '); // "DDS "
}

bool valid_atex(const uint8_t* data_ptr, size_t data_size)
{
	return (data_size >= 4 &&
		data_ptr[0] == 'A' && data_ptr[1] == 'T' &&
		data_ptr[2] == 'E' && data_ptr[3] == 'X'); // "ATEX"
}

bool valid_ateu(const uint8_t* data_ptr, size_t data_size)
{
	return (data_size >= 4 &&
		data_ptr[0] == 'A' && data_ptr[1] == 'T' &&
		data_ptr[2] == 'E' && data_ptr[3] == 'U'); // "ATEU"
}

bool valid_atep(const uint8_t* data_ptr, size_t data_size)
{
	return (data_size >= 4 &&
		data_ptr[0] == 'A' && data_ptr[1] == 'T' &&
		data_ptr[2] == 'E' && data_ptr[3] == 'P'); // "ATEP"
}

bool valid_ctex(const uint8_t* data_ptr, size_t data_size)
{
	return (data_size >= 4 &&
		data_ptr[0] == 'C' && data_ptr[1] == 'T' &&
		data_ptr[2] == 'E' && data_ptr[3] == 'X'); // "CTEX"
}

bool check_valid_image(const uint8_t* data_ptr, size_t data_size)
{
	return valid_png(data_ptr, data_size) || valid_jpeg(data_ptr, data_size) ||
		valid_webp(data_ptr, data_size) || valid_dds(data_ptr, data_size) ||
		valid_atex(data_ptr, data_size) || valid_ateu(data_ptr, data_size) ||
		valid_atep(data_ptr, data_size);
}

bool valid_bink2(const uint8_t* data_ptr, size_t data_size)
{
	// Check for "KB2" (basic Bink2 signature)
	if (data_size >= 4 &&
		data_ptr[0] == 'K' && data_ptr[1] == 'B' &&
		data_ptr[2] == '2')
	{
		return true; // "KB2"
	}
	return false;
}

void check_image_channel(WindowData& window_data)
{
	switch (window_data.image_data.format_data)
	{
		// S3TC (DXT) Formats
	case 0x41545844: // DXTA
	case 0x4C545844: // DXTL
	case 0x55344342: // BC4 (ATI1)
		window_data.image_data.image_channel = 1; // Single-channel (Red)
		break;

	case 0x4E545844: // DXTN
	case 0x55354342: // BC5 (ATI2)
	case 0x58434433: // 3Dc (BC5 variant)
		window_data.image_data.image_channel = 2; // Two-channel (RG)
		break;

	case 0x31545844: // DXT1
	case 0x48364342: // BC6H (HDR)
		window_data.image_data.image_channel = 3; // Three-channel (RGB)
		break;

	case 0x32545844: // DXT2
	case 0x33545844: // DXT3
	case 0x34545844: // DXT4
	case 0x35545844: // DXT5
	case 0x58374342: // BC7
		window_data.image_data.image_channel = 4; // Four-channel (RGBA)
		break;

	default:
		window_data.image_data.image_channel = 0; // Unknown format
		break;
	}
}