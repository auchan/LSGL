#pragma once
#include <cstdint>
#include "..\Math\Math.h"

namespace lsgl
{

	struct Point2D
	{
		Point2D() :x(0), y(0) {}
		Point2D(int64_t _x, int64_t _y) :x(_x), y(_y) {}
		int64_t x;
		int64_t y;

		bool operator==(const Point2D &point)
		{
			return this->x == point.x && this->y == point.y;
		}
	};

	struct Color
	{
		Color(int _r, int _g, int _b, int _a = 1)
			: r(_r), g(_g), b(_b), a(_a)
		{

		}
		int r, g, b, a;
	};

	class RenderBuffer
	{
	public:
		RenderBuffer(size_t width, size_t height, uint8_t pixelBytes = 3);
		~RenderBuffer();

		uint8_t* getData() const;
		__forceinline uint8_t * getPixelUnit(size_t x, size_t y) const
		{
			if (x < 0 || x >= width || y < 0 || y >= height)
			{
				return nullptr;
			}
			size_t startIndex = (width * y + x) * pixelBytes;
			return data + startIndex;
		}

		__forceinline size_t getWidth()
		{
			return width;
		}

		__forceinline size_t getHeight()
		{
			return height;
		}

		size_t getSize()
		{
			return width * height * pixelBytes;
		}

		__forceinline void setByte(size_t x, size_t y, uint8_t val, int offset)
		{
			if (x < 0 || x >= width || y < 0 || y >= height)
			{
				return;
			}

			size_t index = (width * y + x) * pixelBytes + offset;
			data[index] = val;
		}

		__forceinline void setPixelUnit(size_t x, size_t y, uint8_t vals[])
		{
			if (x < 0 || x >= width || y < 0 || y >= height)
			{
				return;
			}

			size_t startIndex = (width * y + x) * pixelBytes;
			size_t size = getSize();
			for (size_t i = 0; i < pixelBytes; ++i)
			{
				size_t index = startIndex + i;
				if (index >= 0 && index < size)
				{
					data[index] = vals[i];
				}
			}
		}
		void clear(Color color = Color(0, 0, 0, 0));
		void clear(uint8_t byte);

		inline Point2D normalizeCoordToScreenCoord(const Vector3& coord)
		{
			return Point2D((coord.x + 1) / 2 * width, (coord.y + 1) / 2 * height);
		}
	private:
		uint8_t* data;
		size_t width;
		size_t height;
		uint8_t pixelBytes;

	};

}