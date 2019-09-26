#include "CameraComponent.h"

namespace lsgl
{
	CameraComponent::CameraComponent()
	{
	}


	CameraComponent::~CameraComponent()
	{
	}

	Matrix4x4 CameraComponent::getViewMatrix()
	{
		return getWorldToLocalTransformMatrix();
	}

	Matrix4x4 CameraComponent::getProjectMatrix()
	{
		return camera.getProjectMatrix();
	}
}