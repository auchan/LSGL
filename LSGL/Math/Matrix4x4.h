#pragma once
#include "Vector4.h"
#include "Vector3.h"
#include "MathUtility.h"
namespace lsgl
{
	// column x row
	class Matrix4x4
	{
	public:
		Matrix4x4();
		Matrix4x4(const Vector4& col1, const Vector4& col2 = Vector4::zero, const Vector4& col3 = Vector4::zero, const Vector4& col4 = Vector4::zero);
		~Matrix4x4();

		Matrix4x4 multiply(const Matrix4x4& mat) const;
		/*
		* @param vec column major vector
		*/
		Vector4 multiply(const Vector4& vec) const;

		Matrix4x4 operator*(const Matrix4x4& mat) const;
		Vector4 operator*(const Vector4& vec) const;
		Vector4 operator*(const Vector3& vec3) const;

	public:
		// 创建一个单位矩阵
		static Matrix4x4 identity();
		// 创建一个平移矩阵
		static Matrix4x4 translate(const Vector3& vec);
		// 创建一个动态旋转矩阵(intrinsic rotations),旋转角度为欧拉角, y-x-z
		// R=Y(\alpha )X(\beta )Z(\gamma )
		static Matrix4x4 rotateEuler(const Vector3& eulerVec);
		// 创建一个静态旋转矩阵(extrinsic rotations),旋转角度为欧拉角, x-y-z
		// R=X(\gamma )Y(\beta )Z(\alpha )
		static Matrix4x4 rotateEulerExtrinsic(const Vector3& eulerVec);
		// 创建一个绕x轴旋转的旋转矩阵
		// @param degree 旋转角度,顺时针
		static Matrix4x4 rotateX(LSFloat degree);
		// 创建一个绕y轴旋转的旋转矩阵
		// @param degree 旋转角度,顺时针
		static Matrix4x4 rotateY(LSFloat degree);
		// 创建一个绕z轴旋转的旋转矩阵
		// @param degree 旋转角度,顺时针
		static Matrix4x4 rotateZ(LSFloat degree);
		// 创建一个缩放矩阵
		static Matrix4x4 scale(const Vector3& vec);

		inline Matrix4x4& transpose();
	public:

		//LSFloat *m;
		LSFloat m11, m21, m31, m41;
		LSFloat m12, m22, m32, m42;
		LSFloat m13, m23, m33, m43;
		LSFloat m14, m24, m34, m44;
	};

	inline Matrix4x4 & Matrix4x4::transpose()
	{
		std::swap(m12, m21);
		std::swap(m13, m31);
		std::swap(m23, m32);
		std::swap(m14, m41);
		std::swap(m24, m42);
		std::swap(m34, m43);
		return *this;
	}
}