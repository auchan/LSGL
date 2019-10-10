#include "Vector2.h"
#include "Vector4.h"
namespace lsgl
{
	Vector2 Vector2::zero = Vector2();
	Vector2 Vector2::one = Vector2(1, 1);

	Vector2::Vector2()
		: x(0)
		, y(0)
	{
	}

	Vector2::Vector2(LSFloat _x, LSFloat _y)
		: x(_x)
		, y(_y)
	{
	}

	Vector2::Vector2(const Vector4 & vec4)
	{
		this->x = vec4.x;
		this->y = vec4.y;
	}

	Vector2::Vector2(const Vector4 && vec4)
	{
		this->x = vec4.x;
		this->y = vec4.y;
	}

	Vector2 Vector2::add(const Vector2 & vec2) const
	{
		Vector2 result;
		return add(*this, vec2, result);
	}

	Vector2 Vector2::substract(const Vector2 & vec2) const
	{
		Vector2 result;
		result.x = this->x - vec2.x;
		result.y = this->y - vec2.y;
		return result;
	}

	Vector2 Vector2::multiply(const Vector2 & vec2) const
	{
		Vector2 result;
		result.x = this->x * vec2.x;
		result.y = this->y * vec2.y;
		return result;
	}

	Vector2 Vector2::multiply(LSFloat scalar) const
	{
		Vector2 result;
		return multiply(*this, scalar, result);
	}

	Vector2 Vector2::divide(LSFloat divisor) const
	{
		Vector2 result;
		result.x = this->x / divisor;
		result.y = this->y / divisor;
		return result;
	}

	LSFloat Vector2::dot(const Vector2 & vec2) const
	{
		LSFloat result = this->x * vec2.x + this->y * vec2.y;
		return result;
	}

	Vector2& Vector2::inverse()
	{
		x = 1 / x;
		y = 1 / y;
		return *this;
	}

	Vector2& Vector2::operator=(const Vector2 & vec2)
	{
		this->x = vec2.x;
		this->y = vec2.y;
		return *this;
	}

	Vector2 Vector2::operator+(const Vector2 & vec2) const
	{
		return add(vec2);
	}

	Vector2 Vector2::operator-(const Vector2 & vec2) const
	{
		return substract(vec2);
	}

	Vector2 Vector2::operator*(const Vector2 & vec2) const
	{
		return multiply(vec2);
	}

	Vector2 Vector2::operator*(LSFloat scalar) const
	{
		return multiply(scalar);
	}

	Vector2 Vector2::operator/(LSFloat divisor) const
	{
		return divide(divisor);
	}
}