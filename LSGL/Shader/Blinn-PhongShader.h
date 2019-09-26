#pragma once
#include "Shader.h"
namespace lsgl
{
	class BlinnPhongShader : public Shader
	{
	public:
		struct UniformBufferLight
		{
			Vector3 pos;
			Vector3 color;
		};

		struct UniformBufferApp
		{
			Vector3 viewPos;
		};

	public:
		BlinnPhongShader() {}

		virtual Vector4 vertexShading() override;
		virtual Vector4 fragmentShading() override;
	};
}