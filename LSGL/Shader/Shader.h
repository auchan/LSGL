#pragma once
#include "Math/Math.h"
namespace lsgl
{
	class Renderer;
	class Shader
	{
	public:
		void setRenderContext(Renderer* inRenderer) {
			renderer = inRenderer;
		}

		virtual Vector4 vertexShading() = 0;
		virtual Vector4 fragmentShading() = 0;
	protected:
		Renderer* renderer;
	};
}