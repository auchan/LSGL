#pragma once
#include "Shader.h"
namespace lsgl
{
	class BlinnPhongShader : public Shader
	{
	public:
		virtual Vector4 vertexShading() override;
		virtual Vector4 fragmentShading() override;
	};
}