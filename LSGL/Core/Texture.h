#pragma once
#include <cstdint>
#include "../Math/Vector4.h"

namespace lsgl
{

	class Texture
	{
	public:
		Texture(uint32_t inWidth, uint32_t inHeight, unsigned char* inData, uint8_t componentNum);
		~Texture();

		Vector4 getColor(uint32_t x, uint32_t y)
		{
			x = x >= width ? width - 1 : x;
			y = y >= height ? height - 1 : y;

			uint32_t pos = (y * height + x) * componentNum;
			return Vector4((float)data[pos + 0] / 255, (float)data[pos + 1] / 255, (float)data[pos + 2] / 255, 1);
		}

	public:
		unsigned char* data;
		uint32_t width;
		uint32_t height;
		uint8_t componentNum;
	};
}