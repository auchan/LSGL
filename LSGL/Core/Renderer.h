#pragma once
#include <memory>
#include "Mesh.h"
#include "RenderBuffer.h"
#include "Sampler.h"
#include "Types.h"

namespace lsgl
{
	class Shader;
	class Renderer
	{
	public:
		Renderer(size_t viewWidth, size_t viewHeight);
		~Renderer();

		void renderVertices(const Vertexes &vertexes, RenderMode renderMode = RenderMode::Surface);
		void renderTriangle(const Vertex &p0, const Vertex &p1, const Vertex &p2, RenderMode renderMode = RenderMode::Surface);
		void clip(Vertexes &vertexes, Vertexes &outputVertexes /* out*/);
		// Is the face be culled?
		bool cullFace(const Vertexes &vertexes);
		void fillPolygon(const Vertexes &vertexes);
		void lineTo(const Vertex &p0, const Vertex &p1);

		void clearBuffers();

		RenderBuffer* getColorBuffer();
		RenderBuffer* getDepthBuffer();

		void setFrontFace(FaceMode mode);

		void setSampler(LayoutBinding binding, const SamplerPtr &sampler);
		SamplerPtr getSampler(LayoutBinding binding);

		void setShader(const std::shared_ptr<Shader>& shader);

		void setDescriptorSet(const std::vector<DescriptorPtr>& descriptors);
		DescriptorPtr getDescriptor(const DescriptorBinding& );
	private:
		RenderBuffer *colorBuffer;
		RenderBuffer *depthBuffer;
		size_t viewWidth;
		size_t viewHeight;
		std::unordered_map<LayoutBinding, SamplerPtr> samplers;
		std::shared_ptr<Shader> shader;
		std::unordered_map<DescriptorBinding, DescriptorPtr> descriptorSet;
		FaceMode frontFace;
	};

}