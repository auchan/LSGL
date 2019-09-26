#pragma once
#define PI 3.141592654
#define degree2radian(degree) degree * PI / 180
#define radian2degree(radian) radian / PI * 180
typedef double LSFloat;

#define vector3_add(lhs, rhs, result) result.x=lhs.x+rhs.x; result.y=lhs.y+rhs.y; result.z=lhs.z+rhs.z;
#define vector3_multiply_scalar(vec3, scalar, result) result.x=vec3.x*scalar; result.y=vec3.y*scalar; result.z=vec3.z*scalar;

#define vertex_add(vertexLeft, vertexRight, result) \
vector3_add(vertexLeft.position, vertexRight.position, result.position); \
vector3_add(vertexLeft.color, vertexRight.color, result.color); \

#define vertex_multiply_scalar(vertex, scalar, result) \
	vector3_multiply_scalar(vertex.position, scalar, result.position); \
	vector3_multiply_scalar(vertex.color, scalar, result.color); \

//other utils
int __cdecl printf2(const char *format, ...);