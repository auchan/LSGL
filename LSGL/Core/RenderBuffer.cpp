#include <cstring>
#include "RenderBuffer.h"

namespace lsgl
{

	RenderBuffer::RenderBuffer(size_t _width, size_t _height, uint8_t _pixelBytes)
		:width(_width)
		, height(_height)
		, pixelBytes(_pixelBytes)
	{
		size_t size = getSize();
		data = new uint8_t[size];
		memset(data, 0, size);
	}

	RenderBuffer::~RenderBuffer()
	{
		delete[] data;
	}

	uint8_t * RenderBuffer::getData() const
	{
		return data;
	}

	void RenderBuffer::clear(Color color)
	{
		//todo
		size_t size = getSize();
		memset(data, 0, size);
	}

	void RenderBuffer::clear(uint8_t byte)
	{
		size_t size = getSize();
		memset(data, byte, size);
	}
}