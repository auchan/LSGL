#pragma once
#include "SceneComponent.h"
#include "..\Core\Camera.h"
#include "..\Math\Matrix4x4.h"

namespace lsgl
{
	class CameraComponent : public SceneComponent
	{
	public:
		CameraComponent();
		~CameraComponent();

		Matrix4x4 getViewMatrix();
		Matrix4x4 getProjectMatrix();

		Camera camera;
	};
}
