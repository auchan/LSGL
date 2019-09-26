#include "StaticMeshComponent.h"

namespace lsgl
{
	StaticMeshComponent::StaticMeshComponent()
	{
	}


	StaticMeshComponent::~StaticMeshComponent()
	{
		for (Mesh* pMesh : meshes)
		{
			delete pMesh;
		}
	}
}
