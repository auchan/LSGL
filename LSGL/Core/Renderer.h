#pragma once
#include "Mesh.h"
#include "RenderBuffer.h"
#include "Sampler.h"
#include "Shader/Shader.h"

namespace lsgl
{

	enum class RenderMode
	{
		Surface,
		Wireframe,
		Count,
	};

	enum class LayoutBinding
	{
		DiffuseTexture,
		SpecularTexture
	};

	class Renderer
	{
	public:
		Renderer(size_t viewWidth, size_t viewHeight);
		~Renderer();

		void renderVertices(const Vertexes &vertexes, const Matrix4x4& M, const Matrix4x4& V, const Matrix4x4& P, RenderMode renderMode = RenderMode::Surface);
		void renderTriangle(const Vertex &p0, const Vertex &p1, const Vertex &p2, const Matrix4x4& M, const Matrix4x4& V, const Matrix4x4& P, RenderMode renderMode = RenderMode::Surface);
		void clip(Vertexes &vertexes, Vertexes &outputVertexes /* out*/);
		void fillPolygon(const Vertexes &vertexes);
		void lineTo(const Vertex &p0, const Vertex &p1);

		void clearBuffers();

		RenderBuffer* getColorBuffer();
		RenderBuffer* getDepthBuffer();

		void setSampler(LayoutBinding binding, Sampler* sampler);
		Sampler* getSampler(LayoutBinding binding);

		void setShader(Shader* shader);
	private:
		RenderBuffer *colorBuffer;
		RenderBuffer *depthBuffer;
		size_t viewWidth;
		size_t viewHeight;
		std::unordered_map<uint32_t, Sampler*> samplers;
		Shader* shader;
	};

}