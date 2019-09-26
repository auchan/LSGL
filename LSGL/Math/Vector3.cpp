#include "Vector3.h"
#include "Vector4.h"
namespace lsgl
{
	Vector3 Vector3::zero = Vector3();
	Vector3 Vector3::one = Vector3(1, 1, 1);

	Vector3::Vector3()
		: x(0)
		, y(0)
		, z(0)
	{
	}

	Vector3::Vector3(LSFloat _x, LSFloat _y, LSFloat _z)
		: x(_x)
		, y(_y)
		, z(_z)
	{
	}

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

	Vector3 Vector3::add(const Vector3 & vec3)
	{
		Vector3 result;
		return add(*this, vec3, result);
	}

	Vector3 Vector3::substract(const Vector3 & vec3)
	{
		Vector3 result;
		result.x = this->x - vec3.x;
		result.y = this->y - vec3.y;
		result.z = this->z - vec3.z;
		return result;
	}

	Vector3 Vector3::multiply(const Vector3 & vec3)
	{
		Vector3 result;
		result.x = this->x * vec3.x;
		result.y = this->y * vec3.y;
		result.z = this->z * vec3.z;
		return result;
	}

	Vector3 Vector3::multiply(LSFloat scalar)
	{
		Vector3 result;
		return multiply(*this, scalar, result);
	}

	Vector3 Vector3::divide(LSFloat divisor)
	{
		Vector3 result;
		result.x = this->x / divisor;
		result.y = this->y / divisor;
		result.z = this->z / divisor;
		return result;
	}

	LSFloat Vector3::dot(const Vector3 & vec3)
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
	Vector3 Vector3::cross(const Vector3 & vec3)
	{
		const Vector3 &a = *this;
		const Vector3 &b = vec3;
		Vector3 result;
		result.x = a.y*b.z - a.z*b.y;
		result.x = a.x*b.z - a.z*b.x;
		result.x = a.x*b.y - a.y*b.x;
		return result;
	}

	Vector3& Vector3::inverse()
	{
		x = 1 / x;
		y = 1 / y;
		z = 1 / z;
		return *this;
	}

	Vector3& Vector3::operator=(const Vector3 & vec3)
	{
		this->x = vec3.x;
		this->y = vec3.y;
		this->z = vec3.z;
		return *this;
	}

	Vector3 Vector3::operator+(const Vector3 & vec3)
	{
		return add(vec3);
	}

	Vector3 Vector3::operator-(const Vector3 & vec3)
	{
		return substract(vec3);
	}

	Vector3 Vector3::operator*(const Vector3 & vec3)
	{
		return multiply(vec3);
	}

	Vector3 Vector3::operator*(LSFloat scalar)
	{
		return multiply(scalar);
	}

	Vector3 Vector3::operator/(LSFloat divisor)
	{
		return divide(divisor);
	}
}