#pragma once
#include <cmath>
#include "MathUtility.h"
namespace lsgl
{
	class Vector4;
	class Vector2
	{
	public:
		Vector2();
		Vector2(LSFloat x, LSFloat y);
		Vector2(const Vector4 &vec4);
		Vector2(const Vector4 &&vec4);

	public:
		Vector2 add(const Vector2 &vec2);
		static Vector2& add(const Vector2 &vecLeft, const Vector2 &vecRight, Vector2 &result);
		Vector2 substract(const Vector2 &vec2);
		static Vector2& substract(const Vector2 &vecLeft, const Vector2 &vecRight, Vector2 &result);
		Vector2 multiply(const Vector2 &vec2);
		Vector2 multiply(LSFloat scalar);
		static Vector2& multiply(const Vector2 &vec2, LSFloat scalar, Vector2 &result);
		Vector2 divide(LSFloat divisor);
		LSFloat dot(const Vector2 &vec2);
		Vector2& inverse();
		LSFloat length()
		{
			return sqrt(x * x + y * y);
		}
		Vector2& normalize()
		{
			// x y z not zero
			if (x == 0 && y == 0)
			{
				return *this;
			}
			LSFloat len = length();
			x /= len;
			y /= len;
			return *this;
		}
	public:
		// operator overwrite
		Vector2& operator=(const Vector2 &vec2);
		Vector2 operator+(const Vector2 &vec2);
		Vector2 operator-(const Vector2 &vec2);
		Vector2 operator*(const Vector2 &vec2);
		Vector2 operator*(LSFloat scalar);
		Vector2 operator/(LSFloat divisor);

	public:
		LSFloat x;
		LSFloat y;
		int *test;

		static Vector2 zero;
		static Vector2 one;
	};

	inline Vector2& Vector2::add(const Vector2 &vecLeft, const Vector2 &vecRight, Vector2 &result)
	{
		result.x = vecLeft.x + vecRight.x;
		result.y = vecLeft.y + vecRight.y;
		return result;
	}

	inline Vector2& Vector2::substract(const Vector2 &vecLeft, const Vector2 &vecRight, Vector2 &result)
	{
		result.x = vecLeft.x - vecRight.x;
		result.y = vecLeft.y - vecRight.y;
		return result;
	}

	inline Vector2& Vector2::multiply(const Vector2 &vec2, LSFloat scalar, Vector2 &result)
	{
		result.x = vec2.x * scalar;
		result.y = vec2.y * scalar;
		return result;
	}
}
