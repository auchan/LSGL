#pragma once
#include <memory>
#include "Math/Math.h"
#include "Core/Vertex.h"
#include "Core/Renderer.h"

namespace lsgl
{
	class Shader
	{
	public:
		struct UniformBufferBase
		{
			Matrix4x4 modelMatrix;
			Matrix4x4 viewMatrix;
			Matrix4x4 projectMatrix;
			Matrix4x4 mvpMatrix;
		};

	public:
		void setRenderContext(Renderer* inRenderer) {
			renderer = inRenderer;
		}

		virtual Vector4 vertexShading() = 0;
		virtual Vector4 fragmentShading() = 0;

		Vertex* vertex;
		Vertex* fragment;
	protected:
		inline Vector4 texture(const DescriptorBinding& binding, const Vector2& uv);

	protected:
		Renderer* renderer;
	};

	using ShaderPtr = std::shared_ptr<Shader>;

	Vector4 Shader::texture(const DescriptorBinding& binding, const Vector2& uv)
	{
		DescriptorPtr descriptor = renderer->getDescriptor(binding);
		if (!descriptor)
		{
			return Vector4();
		}

		//todo validate type
		Sampler* sampler = reinterpret_cast<Sampler*>(descriptor->data);
		return sampler->getSample(uv.x, uv.y);
	}
}