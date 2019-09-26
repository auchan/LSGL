#include "Vector3.h"
#include "Vector4.h"
namespace lsgl
{
	Vector3 Vector3::zero = Vector3();
	Vector3 Vector3::one = Vector3(1, 1, 1);

	Vector3::Vector3(const Vector4 & vec4)
	{
		this->x = vec4.x;
		this->y = vec4.y;
		this->z = vec4.z;
	}

	Vector3::Vector3(const Vector4 && vec4)
	{
		this->x = vec4.x;
		this->y = vec4.y;
		this->z = vec4.z;
	}
}