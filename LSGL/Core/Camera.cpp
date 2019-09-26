#include "Camera.h"
#include "..\Math\Math.h"

namespace lsgl
{

	Camera::Camera()
		:aspectRatio(1)
		, nearPlane(1)
		, farPlane(10000)
		, fov(90)
		, size(100)
		, isOrthographic(false)
	{

	}

	Camera::~Camera()
	{
	}

	Matrix4x4 Camera::getProjectMatrix()
	{
		Matrix4x4 mat;
		if (isOrthographic)
		{
			float width = size * aspectRatio;
			float height = size;
			mat.m11 = 2 / width;
			mat.m22 = 2 / height;
			mat.m33 = 1 / (farPlane - nearPlane);
			mat.m43 = -nearPlane / (farPlane - nearPlane);
			mat.m44 = 1;
		}
		else
		{
			LSFloat halfFovRadian = static_cast<LSFloat>(degree2radian(fov / 2));
			mat.m11 = 1 / (aspectRatio * std::tan(halfFovRadian));
			mat.m22 = 1 / std::tan(halfFovRadian);
			mat.m33 = farPlane / (farPlane - nearPlane);
			mat.m34 = 1;
			mat.m43 = -farPlane * nearPlane / (farPlane - nearPlane);
			mat.m44 = 0;
		}
		return mat;
	}
}