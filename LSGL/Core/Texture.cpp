#include "Texture.h"

namespace lsgl
{

	Texture::Texture(uint32_t inWidth, uint32_t inHeight, unsigned char* inData, uint8_t inComponentNum)
		: width(inWidth)
		, height(inHeight)
		, data(inData)
		, componentNum(inComponentNum)
	{
	}

	Texture::~Texture()
	{
		if (data)
		{
			delete[] data;
			data = nullptr;
		}
	}

}