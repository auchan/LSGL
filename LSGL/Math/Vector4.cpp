#include "Vector4.h"
namespace lsgl
{
	Vector4 Vector4::zero = Vector4();
	Vector4 Vector4::one = Vector4(1, 1, 1, 1);

	Vector4::Vector4()
		: x(0)
		, y(0)
		, z(0)
		, w(0)
	{
	}

	Vector4::Vector4(LSFloat _x, LSFloat _y, LSFloat _z, LSFloat _w)
		: x(_x)
		, y(_y)
		, z(_z)
		, w(_w)
	{
	}

	Vector4::Vector4(const Vector3 vec3, LSFloat w)
	{
		this->x = vec3.x;
		this->y = vec3.y;
		this->z = vec3.z;
		this->w = w;
	}

	Vector4::~Vector4()
	{
	}

	Vector4 Vector4::add(const Vector4 & vec4)
	{
		Vector4 result;
		result.x = this->x + vec4.x;
		result.y = this->y + vec4.y;
		result.z = this->z + vec4.z;
		result.w = this->w + vec4.w;
		return result;
	}

	Vector4 Vector4::substract(const Vector4 & vec4)
	{
		Vector4 result;
		result.x = this->x - vec4.x;
		result.y = this->y - vec4.y;
		result.z = this->z - vec4.z;
		result.w = this->w - vec4.w;
		return result;
	}

	Vector4 Vector4::multiply(const Vector4 & vec4)
	{
		Vector4 result;
		result.x = this->x * vec4.x;
		result.y = this->y * vec4.y;
		result.z = this->z * vec4.z;
		result.w = this->w * vec4.w;
		return result;
	}

	Vector4 Vector4::multiply(LSFloat scalar)
	{
		Vector4 result;
		result.x = this->x * scalar;
		result.y = this->y * scalar;
		result.z = this->z * scalar;
		result.w = this->w * scalar;
		return result;
	}

	Vector4 Vector4::divide(LSFloat divisor)
	{
		Vector4 result;
		result.x = this->x / divisor;
		result.y = this->y / divisor;
		result.z = this->z / divisor;
		result.w = this->w / divisor;
		return result;
	}

	LSFloat Vector4::dot(const Vector4 & vec4)
	{
		LSFloat result = this->x * vec4.x + this->y * vec4.y + this->z * vec4.z + this->w * vec4.w;
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
	Vector4 Vector4::cross(const Vector4 & vec4)
	{
		const Vector4 &a = *this;
		const Vector4 &b = vec4;
		Vector4 result;
		result.x = a.y*b.z - a.z*b.y;
		result.x = a.x*b.z - a.z*b.x;
		result.x = a.x*b.y - a.y*b.x;
		result.w = 1;
		return result;
	}

	Vector4 Vector4::operator+(const Vector4 & vec4)
	{
		return add(vec4);
	}

	Vector4 Vector4::operator-(const Vector4 & vec4)
	{
		return substract(vec4);
	}

	Vector4 Vector4::operator*(const Vector4 & vec4)
	{
		return multiply(vec4);
	}

	Vector4 Vector4::operator*(LSFloat scalar)
	{
		return multiply(scalar);
	}

	Vector4 Vector4::operator/(LSFloat divisor)
	{
		return divide(divisor);
	}

	std::ostream & operator<<(std::ostream & os, Vector4 &vec4)
	{
		os << "x: " << vec4.x << ", y: " << vec4.y << ", z: " << vec4.z << ", w: " << vec4.w;
		return os;
	}
}