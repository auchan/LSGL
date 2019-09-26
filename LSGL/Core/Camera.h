#pragma once
#include "..\Math\Matrix4x4.h"
namespace lsgl
{

	class Camera
	{
	public:
		Camera();
		~Camera();

		Matrix4x4 getProjectMatrix();

		float aspectRatio;
		float nearPlane;
		float farPlane;
		//透视投影
		float fov;
		//正交投影
		float size;

		bool isOrthographic;
	};

}