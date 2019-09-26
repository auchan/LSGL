#pragma once
#include <cmath>
#include "MathUtility.h"
namespace lsgl
{
	class Vector4;
	class Vector3
	{
	public:
		Vector3();
		Vector3(LSFloat x, LSFloat y, LSFloat z);
		Vector3(const Vector4 &vec4);
		Vector3(const Vector4 &&vec4);

	public:
		Vector3 add(const Vector3 &vec3);
		static Vector3& add(const Vector3 &vecLeft, const Vector3 &vecRight, Vector3 &result);
		Vector3 substract(const Vector3 &vec3);
		Vector3 multiply(const Vector3 &vec3);
		Vector3 multiply(LSFloat scalar);
		static Vector3& multiply(const Vector3 &vec3, LSFloat scalar, Vector3 &result);
		Vector3 divide(LSFloat divisor);
		LSFloat dot(const Vector3 &vec3);
		Vector3 cross(const Vector3 &vec3);
		Vector3& inverse();
		LSFloat length()
		{
			return std::sqrtf(x * x + y * y + z * z);
		}
		Vector3& normalize()
		{
			// x y z not zero
			if (x == 0 && y == 0 && z == 0)
			{
				return *this;
			}
			LSFloat len = length();
			x /= len;
			y /= len;
			z /= len;
			return *this;
		}
	public:
		// operator overwrite
		Vector3& operator=(const Vector3 &vec3);
		Vector3 operator+(const Vector3 &vec3);
		Vector3 operator-(const Vector3 &vec3);
		Vector3 operator*(const Vector3 &vec3);
		Vector3 operator*(LSFloat scalar);
		Vector3 operator/(LSFloat divisor);

	public:
		LSFloat x;
		LSFloat y;
		LSFloat z;
		int *test;

		static Vector3 zero;
		static Vector3 one;
	};

	inline Vector3& Vector3::add(const Vector3 &vecLeft, const Vector3 &vecRight, Vector3 &result)
	{
		result.x = vecLeft.x + vecRight.x;
		result.y = vecLeft.y + vecRight.y;
		result.z = vecLeft.z + vecRight.z;
		return result;
	}

	inline Vector3& Vector3::multiply(const Vector3 &vec3, LSFloat scalar, Vector3 &result)
	{
		result.x = vec3.x * scalar;
		result.y = vec3.y * scalar;
		result.z = vec3.z * scalar;
		return result;
	}
}
