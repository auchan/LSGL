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
		//͸��ͶӰ
		float fov;
		//����ͶӰ
		float size;

		bool isOrthographic;
	};

}