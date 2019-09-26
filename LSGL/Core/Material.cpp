#include "Material.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stbimage/stb_image.h>
#include <assimp/material.h>

namespace lsgl
{
	Material::Material()
	{

	}

	Material::~Material()
	{

	}

	Texture* Material::getTexture(MaterialTextureType textureType, uint32_t index)
	{
		if (rawMaterial == nullptr)
		{
			return nullptr;
		}

		uint32_t materialId = (static_cast<uint16_t>(textureType) << 4) + index;
		auto findResult = textureCaches.find(materialId);
		if (findResult != textureCaches.end())
		{
			return findResult->second;
		}

		Texture* pTexture = nullptr;
		if (textureType == MaterialTextureType::Diffuse)
		{
			aiString diffuseTexturePath;
			if (rawMaterial->GetTexture(aiTextureType::aiTextureType_DIFFUSE, index, &diffuseTexturePath) == aiReturn::aiReturn_SUCCESS)
			{
				int width, height, compInFile;
				int comp = 3;
				stbi_uc* imageData = stbi_load((modelFileBaseDir + "/" + diffuseTexturePath.C_Str()).c_str(), &width, &height, &compInFile, comp);
				pTexture = new Texture(width, height, imageData, static_cast<uint8_t>(comp));
			}
		}
		else if(textureType == MaterialTextureType::Specular)
		{
			aiString specularTexturePath;
			if (rawMaterial->GetTexture(aiTextureType::aiTextureType_SPECULAR, 0, &specularTexturePath) == aiReturn::aiReturn_SUCCESS)
			{
				int width, height, compInFile;
				int comp = 1;
				stbi_uc* imageData = stbi_load((modelFileBaseDir + "/" + specularTexturePath.C_Str()).c_str(), &width, &height, &compInFile, comp);
				pTexture = new Texture(width, height, imageData, static_cast<uint8_t>(comp));
			}
		}

		if (pTexture)
		{
			textureCaches[materialId] = pTexture;
		}
		return pTexture;
	}
}