#pragma once
#include "../Math/Vector3.h"
namespace lsgl
{
	class Transform
	{
	public:
		Transform();
		~Transform();
	public:
		Vector3 position;
		Vector3 rotation;
		Vector3 scale;
	};
}