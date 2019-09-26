#include <cstring>
#include <cmath>
#include "Matrix4x4.h"
#include "MathUtility.h"
namespace lsgl
{
	Matrix4x4::Matrix4x4()
	{
		memset(&m11, 0, sizeof(LSFloat) * 16);
	}

	Matrix4x4::Matrix4x4(const Vector4 & col1, const Vector4 & col2, const Vector4 & col3, const Vector4 & col4)
	{
		m11 = col1.x; m21 = col2.x; m31 = col3.x; m41 = col4.x;
		m12 = col1.y; m22 = col2.y; m32 = col3.y; m42 = col4.z;
		m13 = col1.z; m23 = col2.z; m33 = col3.z; m43 = col4.y;
		m14 = col1.w; m24 = col2.w; m34 = col3.w; m44 = col4.w;
	}

	Matrix4x4::~Matrix4x4()
	{
	}

	Matrix4x4 Matrix4x4::multiply(const Matrix4x4 & mat) const
	{
		Matrix4x4 result;

		result.m11 = m11 * mat.m11 + m21 * mat.m12 + m31 * mat.m13 + m41 * mat.m14;
		result.m12 = m12 * mat.m11 + m22 * mat.m12 + m32 * mat.m13 + m42 * mat.m14;
		result.m13 = m13 * mat.m11 + m23 * mat.m12 + m33 * mat.m13 + m43 * mat.m14;
		result.m14 = m14 * mat.m11 + m24 * mat.m12 + m34 * mat.m13 + m44 * mat.m14;

		result.m21 = m11 * mat.m21 + m21 * mat.m22 + m31 * mat.m23 + m41 * mat.m24;
		result.m22 = m12 * mat.m21 + m22 * mat.m22 + m32 * mat.m23 + m42 * mat.m24;
		result.m23 = m13 * mat.m21 + m23 * mat.m22 + m33 * mat.m23 + m43 * mat.m24;
		result.m24 = m14 * mat.m21 + m24 * mat.m22 + m34 * mat.m23 + m44 * mat.m24;

		result.m31 = m11 * mat.m31 + m21 * mat.m32 + m31 * mat.m33 + m41 * mat.m34;
		result.m32 = m12 * mat.m31 + m22 * mat.m32 + m32 * mat.m33 + m42 * mat.m34;
		result.m33 = m13 * mat.m31 + m23 * mat.m32 + m33 * mat.m33 + m43 * mat.m34;
		result.m34 = m14 * mat.m31 + m24 * mat.m32 + m34 * mat.m33 + m44 * mat.m34;

		result.m41 = m11 * mat.m41 + m21 * mat.m42 + m31 * mat.m43 + m41 * mat.m44;
		result.m42 = m12 * mat.m41 + m22 * mat.m42 + m32 * mat.m43 + m42 * mat.m44;
		result.m43 = m13 * mat.m41 + m23 * mat.m42 + m33 * mat.m43 + m43 * mat.m44;
		result.m44 = m14 * mat.m41 + m24 * mat.m42 + m34 * mat.m43 + m44 * mat.m44;

		return result;
	}

	Vector4 Matrix4x4::multiply(const Vector4& vec) const
	{
		Vector4 result;
		result.x = m11 * vec.x + m21 * vec.y + m31 * vec.z + m41 * vec.w;
		result.y = m12 * vec.x + m22 * vec.y + m32 * vec.z + m42 * vec.w;
		result.z = m13 * vec.x + m23 * vec.y + m33 * vec.z + m43 * vec.w;
		result.w = m14 * vec.x + m24 * vec.y + m34 * vec.z + m44 * vec.w;
		return result;
	}

	Matrix4x4 Matrix4x4::operator*(const Matrix4x4 & mat) const
	{
		return this->multiply(mat);
	}

	Vector4 Matrix4x4::operator*(const Vector4 & vec) const
	{
		return this->multiply(vec);
	}

	Vector4 Matrix4x4::operator*(const Vector3 & vec3) const
	{
		return this->multiply(Vector4(vec3, 1));
	}

	Matrix4x4 Matrix4x4::identity()
	{
		return Matrix4x4(Vector4(1, 0, 0, 0), Vector4(0, 1, 0, 0), Vector4(0, 0, 1, 0), Vector4(0, 0, 0, 1));
	}

	Matrix4x4 Matrix4x4::translate(const Vector3 & vec)
	{
		Matrix4x4 mat = Matrix4x4::identity();
		mat.m41 = vec.x;
		mat.m42 = vec.y;
		mat.m43 = vec.z;
		return mat;
	}

	Matrix4x4 Matrix4x4::rotateEuler(const Vector3 & eulerVec)
	{
		// todo: use pre calculated formula
		Matrix4x4 mat = Matrix4x4::rotateY(eulerVec.y) *  Matrix4x4::rotateX(eulerVec.x) *  Matrix4x4::rotateZ(eulerVec.z);
		return mat;
	}

	Matrix4x4 Matrix4x4::rotateEulerExtrinsic(const Vector3 & eulerVec)
	{
		// todo: use pre calculated formula
		Matrix4x4 mat = Matrix4x4::rotateX(eulerVec.x) *  Matrix4x4::rotateY(eulerVec.y) *  Matrix4x4::rotateZ(eulerVec.z);
		return mat;
	}

	Matrix4x4 Matrix4x4::rotateX(LSFloat degree)
	{
		Matrix4x4 mat = Matrix4x4::identity();
		LSFloat radian = degree2radian(degree);
		//左手坐标系
		mat.m22 = std::cos(radian); mat.m32 = -std::sin(radian);
		mat.m23 = std::sin(radian); mat.m33 = std::cos(radian);
		return mat;
	}

	Matrix4x4 Matrix4x4::rotateY(LSFloat degree)
	{
		Matrix4x4 mat = Matrix4x4::identity();
		LSFloat radian = degree2radian(degree);
		//左手坐标系
		mat.m11 = std::cos(radian); mat.m31 = std::sin(radian);
		mat.m13 = -std::sin(radian); mat.m33 = std::cos(radian);
		return mat;
	}

	Matrix4x4 Matrix4x4::rotateZ(LSFloat degree)
	{
		Matrix4x4 mat = Matrix4x4::identity();
		LSFloat radian = degree2radian(degree);
		//左手坐标系
		mat.m11 = std::cos(radian); mat.m21 = -std::sin(radian);
		mat.m12 = std::sin(radian); mat.m22 = std::cos(radian);
		return mat;
	}

	Matrix4x4 Matrix4x4::scale(const Vector3 & vec)
	{
		Matrix4x4 mat = Matrix4x4::identity();
		mat.m11 = vec.x;
		mat.m22 = vec.y;
		mat.m33 = vec.z;
		return mat;
	}
}