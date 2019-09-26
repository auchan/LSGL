#pragma once
#include "SceneComponent.h"
#include "../Core/Mesh.h"

namespace lsgl
{
	class StaticMeshComponent : public SceneComponent
	{
	public:
		StaticMeshComponent();
		~StaticMeshComponent();
	public:
		std::vector<Mesh*> meshes;
	};
}


