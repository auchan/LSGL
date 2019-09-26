#pragma once
#include <cstdint>
#include <unordered_map>
#include "../Math/Vector4.h"
#include "Texture.h"

struct aiMaterial;
namespace lsgl
{
	enum class MaterialTextureType : uint8_t
	{
		Diffuse,
		Specular,
		Normal,
	};

	class Material
	{
	public:
		Material();
		~Material();
		Texture* getTexture(MaterialTextureType textureType, uint32_t index);
	public:
		aiMaterial* rawMaterial;
		std::string modelFileBaseDir;
	private:
		std::unordered_map<uint32_t, Texture*> textureCaches;
	};
}

