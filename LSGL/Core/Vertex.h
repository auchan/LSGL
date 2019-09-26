#pragma once
#include <vector>
#include "../Math/Math.h"

namespace lsgl
{
	struct Vertex
	{
		inline static Vertex& add(const Vertex &vertexLeft, const Vertex &vertexRight, Vertex &result);
		inline static Vertex& substract(const Vertex &vertexLeft, const Vertex &vertexRight, Vertex &result);
		inline static Vertex& multiply(const Vertex &vertex, LSFloat scalar, Vertex &result);

		Vector4 position;
		Vector4 color;
		Vector2 uv;
		//Vector3 normal;
	};
	typedef std::vector<Vertex> Vertexes;

	inline Vertex & Vertex::add(const Vertex & vertexLeft, const Vertex & vertexRight, Vertex & result)
	{
		Vector4::add(vertexLeft.position, vertexRight.position, result.position);
		Vector4::add(vertexLeft.color, vertexRight.color, result.color);
		Vector2::add(vertexLeft.uv, vertexRight.uv, result.uv);
		return result;
	}

	inline Vertex & Vertex::substract(const Vertex & vertexLeft, const Vertex & vertexRight, Vertex & result)
	{
		Vector4::substract(vertexLeft.position, vertexRight.position, result.position);
		Vector4::substract(vertexLeft.color, vertexRight.color, result.color);
		Vector2::substract(vertexLeft.uv, vertexRight.uv, result.uv);
		return result;
	}

	inline Vertex & Vertex::multiply(const Vertex & vertex, LSFloat scalar, Vertex & result)
	{
		Vector4::multiply(vertex.position, scalar, result.position);
		Vector4::multiply(vertex.color, scalar, result.color);
		Vector2::multiply(vertex.uv, scalar, result.uv);
		return result;
	}
}