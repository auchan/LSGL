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
		inline Vector3 add(const Vector3 &vec3) const;
		inline static Vector3& add(const Vector3 &vecLeft, const Vector3 &vecRight, Vector3 &result);
		inline Vector3 substract(const Vector3 &vec3) const;
		inline static Vector3& substract(const Vector3 &vecLeft, const Vector3 &vecRight, Vector3 &result);
		inline Vector3 multiply(const Vector3 &vec3) const;
		inline Vector3 multiply(LSFloat scalar) const;
		inline static Vector3& multiply(const Vector3 &vec3, LSFloat scalar, Vector3 &result);
		inline Vector3 divide(LSFloat divisor) const;
		inline LSFloat dot(const Vector3 &vec3) const;
		inline Vector3 cross(const Vector3 &vec3) const;
		inline Vector3& inverse();
		LSFloat length() const
		{
			return sqrt(lengthSquared());
		}
		LSFloat lengthSquared() const
		{
			return x * x + y * y + z * z;
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
		inline Vector3& operator=(const Vector3 &vec3);
		inline Vector3 operator+(const Vector3 &vec3) const;
		inline Vector3 operator-(const Vector3 &vec3) const;
		inline Vector3 operator*(const Vector3 &vec3) const;
		inline Vector3 operator*(LSFloat scalar) const;
		inline Vector3 operator/(LSFloat divisor) const;

	public:
		LSFloat x;
		LSFloat y;
		LSFloat z;

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

	inline Vector3& Vector3::substract(const Vector3 &vecLeft, const Vector3 &vecRight, Vector3 &result)
	{
		result.x = vecLeft.x - vecRight.x;
		result.y = vecLeft.y - vecRight.y;
		result.z = vecLeft.z - vecRight.z;
		return result;
	}

	inline Vector3& Vector3::multiply(const Vector3 &vec3, LSFloat scalar, Vector3 &result)
	{
		result.x = vec3.x * scalar;
		result.y = vec3.y * scalar;
		result.z = vec3.z * scalar;
		return result;
	}

	inline Vector3::Vector3()
		: x(0)
		, y(0)
		, z(0)
	{
	}

	inline Vector3::Vector3(LSFloat _x, LSFloat _y, LSFloat _z)
		: x(_x)
		, y(_y)
		, z(_z)
	{
	}

	inline Vector3 Vector3::add(const Vector3 & vec3) const
	{
		Vector3 result;
		return add(*this, vec3, result);
	}

	inline Vector3 Vector3::substract(const Vector3 & vec3) const
	{
		Vector3 result;
		result.x = this->x - vec3.x;
		result.y = this->y - vec3.y;
		result.z = this->z - vec3.z;
		return result;
	}

	inline Vector3 Vector3::multiply(const Vector3 & vec3) const
	{
		Vector3 result;
		result.x = this->x * vec3.x;
		result.y = this->y * vec3.y;
		result.z = this->z * vec3.z;
		return result;
	}

	inline Vector3 Vector3::multiply(LSFloat scalar) const
	{
		Vector3 result;
		return multiply(*this, scalar, result);
	}

	inline Vector3 Vector3::divide(LSFloat divisor) const
	{
		Vector3 result;
		result.x = this->x / divisor;
		result.y = this->y / divisor;
		result.z = this->z / divisor;
		return result;
	}

	inline LSFloat Vector3::dot(const Vector3 & vec3) const
	{
		LSFloat result = this->x * vec3.x + this->y * vec3.y + this->z * vec3.z;
		return result;
	}

	/*
				|i   j   k  |
		a x b = |a.x a.y a.z|
				|b.x b.y b.z|
			  = |a.y a.z|     |a.x a.z|     |a.x a.y|
				|b.y b.z|*i + |b.x b.z|*j + |b.x b.y|*k
			  = (a.y*b.z - a.z*b.y)i - (a.x*b.z - a.z*b.x)j + (a.x*b.y - a.y*b.x)k
	*/
	inline Vector3 Vector3::cross(const Vector3 & vec3) const
	{
		const Vector3 &a = *this;
		const Vector3 &b = vec3;
		Vector3 result;
		result.x = a.y*b.z - a.z*b.y;
		result.y = a.x*b.z - a.z*b.x;
		result.z = a.x*b.y - a.y*b.x;
		return result;
	}

	inline Vector3& Vector3::inverse()
	{
		x = 1 / x;
		y = 1 / y;
		z = 1 / z;
		return *this;
	}

	inline Vector3& Vector3::operator=(const Vector3 & vec3)
	{
		this->x = vec3.x;
		this->y = vec3.y;
		this->z = vec3.z;
		return *this;
	}

	inline Vector3 Vector3::operator+(const Vector3 & vec3) const
	{
		return add(vec3);
	}

	inline Vector3 Vector3::operator-(const Vector3 & vec3) const
	{
		return substract(vec3);
	}

	inline Vector3 Vector3::operator*(const Vector3 & vec3) const
	{
		return multiply(vec3);
	}

	inline Vector3 Vector3::operator*(LSFloat scalar) const
	{
		return multiply(scalar);
	}

	inline Vector3 Vector3::operator/(LSFloat divisor) const
	{
		return divide(divisor);
	}
}
