#pragma once
#include <iostream>
#include "MathUtility.h"
#include "Vector3.h"
namespace lsgl
{
	class Vector4
	{
	public:
		Vector4();
		Vector4(LSFloat x, LSFloat y, LSFloat z, LSFloat w);
		Vector4(const Vector3 vec3, LSFloat w);
		~Vector4();

	public:
		Vector4 add(const Vector4 &vec4) const;
		Vector4 substract(const Vector4 &vec4) const;
		Vector4 multiply(const Vector4 &vec4) const;
		Vector4 multiply(LSFloat scalar) const;
		Vector4 divide(LSFloat divisor) const;
		LSFloat dot(const Vector4 &vec4) const;
		Vector4 cross(const Vector4 &vec4) const;

		inline static Vector4& add(const Vector4 &vecLeft, const Vector4 &vecRight, Vector4 &result);
		inline static Vector4& substract(const Vector4 &vecLeft, const Vector4 &vecRight, Vector4 &result);
		inline static Vector4& multiply(const Vector4 &vec4, LSFloat scalar, Vector4 &result);
	public:
		// operator overwrite
		Vector4 operator+(const Vector4 &vec4) const;
		Vector4 operator-(const Vector4 &vec4) const;
		Vector4 operator*(const Vector4 &vec4) const;
		Vector4 operator*(LSFloat scalar) const;
		Vector4 operator/(LSFloat divisor) const;

		friend std::ostream& operator<<(std::ostream &os, Vector4 &vec4);

	public:
		LSFloat x;
		LSFloat y;
		LSFloat z;
		LSFloat w;

		static Vector4 zero;
		static Vector4 one;
	};

	inline Vector4& Vector4::add(const Vector4 &vecLeft, const Vector4 &vecRight, Vector4 &result)
	{
		result.x = vecLeft.x + vecRight.x;
		result.y = vecLeft.y + vecRight.y;
		result.z = vecLeft.z + vecRight.z;
		result.w = vecLeft.w + vecRight.w;
		return result;
	}

	inline Vector4& Vector4::substract(const Vector4 &vecLeft, const Vector4 &vecRight, Vector4 &result)
	{
		result.x = vecLeft.x - vecRight.x;
		result.y = vecLeft.y - vecRight.y;
		result.z = vecLeft.z - vecRight.z;
		result.w = vecLeft.w - vecRight.w;
		return result;
	}

	inline Vector4& Vector4::multiply(const Vector4 &vec4, LSFloat scalar, Vector4 &result)
	{
		result.x = vec4.x * scalar;
		result.y = vec4.y * scalar;
		result.z = vec4.z * scalar;
		result.w = vec4.w * scalar;
		return result;
	}
}