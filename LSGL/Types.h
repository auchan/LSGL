#pragma once
#include <cstdint>
#include <string>

namespace lsgl
{
	using LayoutBinding = uint32_t;
	using DescriptorBinding = std::string;
	enum RenderMode
	{
		Surface,
		Wireframe,
		Count,
	};

	enum class PiplineStageBit : uint8_t
	{
		Vertex = 0x1,
		Fragment = 0x2,
	};

	struct Descriptor
	{
		DescriptorBinding binding;
		void* data;
		PiplineStageBit stage;
	};
	using DescriptorPtr = std::shared_ptr<Descriptor>;
}