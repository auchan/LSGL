#pragma once
#include "Texture.h"

namespace lsgl
{

	class Sampler
	{
	public:
		Sampler(Texture* inTexture);

		void setTexture(Texture* inTexture)
		{
			pTexture = inTexture;
		}

		Vector4 getSample(float x, float y)
		{
			if (!pTexture)
			{
				return Vector4(0.0f, 0.0f, 1.0f, 1.0f);
			}

			x = x < 0 ? 0 : x;
			x = x > 1 ? 1 : x;
			y = y < 0 ? 0 : y;
			y = y > 1 ? 1 : y;

			uint32_t intX = x * pTexture->width;
			uint32_t intY = y * pTexture->height;
			return pTexture->getColor(intX, intY);
		}
	private:
		Texture* pTexture;
	};

}