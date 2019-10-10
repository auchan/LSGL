#include <algorithm>
#include <map>
#include <ctime>

#include "Renderer.h"
#include "Shader/Shader.h"

namespace lsgl
{

	typedef uint32_t DepthFormat;
#define DEPTH_MAX UINT32_MAX
#define DEPTH_FORMAT_BYTES 4
	static uint32_t setPixelCount = 0;
	// clip bit code
	uint8_t leftBitCode = 0x1;
	uint8_t rightBitCode = 0x2;
	uint8_t bottomBitCode = 0x4;
	uint8_t topBitCode = 0x8;
	uint8_t nearBitCode = 0x10;
	uint8_t farBitCode = 0x20;

	Renderer::Renderer(size_t _viewWidth, size_t _viewHeight)
		: viewWidth(_viewWidth)
		, viewHeight(_viewHeight)
		, shader(nullptr)
		, frontFace(FaceMode::CCW)
	{
		colorBuffer = new RenderBuffer(viewWidth, viewHeight);
		depthBuffer = new RenderBuffer(viewWidth, viewHeight, DEPTH_FORMAT_BYTES);
		depthBuffer->clear(0xff);
	}

	Renderer::~Renderer()
	{
		delete colorBuffer;
		delete depthBuffer;
	}

	void Renderer::renderVertices(const Vertexes &vertices, RenderMode renderMode)
	{
		clock_t beginTime = clock();
		setPixelCount = 0;
		size_t size = vertices.size();
		// primitive: only support triangle
		for (size_t i = 0; i < size; i += 3)
		{
			if (i + 2 >= size)
			{
				break;
			}

			renderTriangle(vertices[i], vertices[i + 1], vertices[i + 2], renderMode);
		}
		clock_t endTime = clock();
		LSFloat deltaTime = (LSFloat)(endTime - beginTime) * 1000 / CLOCKS_PER_SEC;
		//printf2("render use %f ms\n", deltaTime);
	}

	void Renderer::renderTriangle(const Vertex & p0, const Vertex & p1, const Vertex & p2, RenderMode renderMode)
	{
		Vertex np0 = p0, np1 = p1, np2 = p2;
		if (shader)
		{
			shader->vertex = &np0;
			np0.position = shader->vertexShading();
			shader->vertex = &np1;
			np1.position = shader->vertexShading();
			shader->vertex = &np2;
			np2.position = shader->vertexShading();
		}
		else
		{
			//todo: ensure valid
			Shader::UniformBufferBase* baseUniformBuffer = reinterpret_cast<Shader::UniformBufferBase*>(getDescriptor("_Base")->data);
			Matrix4x4 &mvpMat = baseUniformBuffer->mvpMatrix;

			np0.position = (mvpMat) * p0.position;
			np1.position = (mvpMat) * p1.position;
			np2.position = (mvpMat) * p2.position;
		}
		Vertexes vertexesInput = { np0, np1, np2 };
		Vertexes vertexes;
		clip(vertexesInput, vertexes);
		// perspective divide
		for (Vertex &vertex : vertexes)
		{
			vertex.position = vertex.position / vertex.position.w;
		}

		if (cullFace(vertexes))
		{
			return;
		}

		if (renderMode == RenderMode::Surface)
		{

			fillPolygon(vertexes);
		}
		else
		{
			for (auto cur = vertexes.begin(); cur != vertexes.end(); ++cur)
			{
				auto next = cur + 1;
				if ((cur + 1) == vertexes.end())
				{
					next = vertexes.begin();
				}
				lineTo(*cur, *next);
			}
		}
	}

	/*
	* Clip without perspective correct
	*/
	void clipProcess(Vertexes & vertexes, int stage, Vertexes & outputVertexes)
	{
		if (stage > 6)
		{
			outputVertexes = vertexes;
			return;
		}

		uint8_t regionCode0 = 0, regionCode1 = 0;
		auto begin = vertexes.begin();
		auto end = vertexes.end();
		Vertexes innerVertexes = Vertexes();
		for (auto cur = begin; cur != end; ++cur)
		{
			Vertex &v0 = *cur;
			auto tmpIter = begin;
			if ((cur + 1) != end)
			{
				tmpIter = cur + 1;
			}
			Vertex &v1 = *tmpIter;

			Vector4 &pos0 = v0.position;
			Vector4 &pos1 = v1.position;
			LSFloat clipAlpha = 0.0f;
			LSFloat absw0 = std::fabs(pos0.w);
			LSFloat absw1 = std::fabs(pos1.w);

			bool w0Valid = absw0 > 1e-6;
			if (w0Valid)
			{
				pos0.x /= absw0;
				pos0.y /= absw0;
				pos0.z /= absw0;
			}
			else
			{
				pos0.x = pos0.x > 1e-6 ? 1 : (pos0.x < -1e-6 ? -1 : 0);
				pos0.y = pos0.y > 1e-6 ? 1 : (pos0.y < -1e-6 ? -1 : 0);
				pos0.z = 0;
			}
			pos0.w = absw0 = 1;

			bool w1Valid = absw1 > 1e-6;
			if (w1Valid)
			{
				pos1.x /= absw1;
				pos1.y /= absw1;
				pos1.z /= absw1;
			}
			else
			{
				pos1.x = pos1.x > 1e-6 ? 1 : (pos1.x < -1e-6 ? -1 : 0);
				pos1.y = pos1.y > 1e-6 ? 1 : (pos1.y < -1e-6 ? -1 : 0);
				pos1.z = 0;
			}
			pos1.w = absw1 = 1;

			int sign0 = 1;// pos0.w >= 0 ? 1 : -1;
			int sign1 = 1;// pos1.w >= 0 ? 1 : -1;
			LSFloat x0 = sign0 * pos0.x;
			LSFloat x1 = sign1 * pos1.x;
			LSFloat y0 = sign0 * pos0.y;
			LSFloat y1 = sign1 * pos1.y;
			LSFloat z0 = sign0 * pos0.z;
			LSFloat z1 = sign1 * pos1.z;

			switch (stage)
			{
			case 1:
				regionCode0 = (x0 <= -absw0 ? leftBitCode : 0);
				regionCode1 = (x1 <= -absw1 ? leftBitCode : 0);
				if (regionCode0 != regionCode1)
				{
					clipAlpha = (-absw0 - x0) / (x1 - x0);
				}
				break;
			case 2:
				regionCode0 = (x0 >= absw0 ? rightBitCode : 0);
				regionCode1 = (x1 >= absw1 ? rightBitCode : 0);
				if (regionCode0 != regionCode1)
				{
					clipAlpha = (absw0 - x0) / (x1 - x0);
				}
				break;
			case 3:
				regionCode0 = (y0 <= -absw0 ? bottomBitCode : 0);
				regionCode1 = (y1 <= -absw1 ? bottomBitCode : 0);
				if (regionCode0 != regionCode1)
				{
					clipAlpha = (-absw0 - y0) / (y1 - y0);
				}
				break;
			case 4:
				regionCode0 = (y0 >= absw0 ? topBitCode : 0);
				regionCode1 = (y1 >= absw1 ? topBitCode : 0);
				if (regionCode0 != regionCode1)
				{
					clipAlpha = (absw0 - y0) / (y1 - y0);
				}
				break;
			case 5:
				regionCode0 = (z0 <= 0 ? nearBitCode : 0);
				regionCode1 = (z1 <= 0 ? nearBitCode : 0);
				if (regionCode0 != regionCode1)
				{
					clipAlpha = (0 - z0) / (z1 - z0);
				}
				break;
			case 6:
				regionCode0 = (z0 >= absw0 ? topBitCode : 0);
				regionCode1 = (z1 >= absw1 ? topBitCode : 0);
				if ((regionCode0 ^ regionCode1) != 0)
				{
					clipAlpha = (absw0 - z0) / (z1 - z0);
				}
				break;
			default:
				break;
			}

			Vertex v2 = v0;
			Vertex::add(v0, Vertex::multiply(Vertex::substract(v1, v0, v2), clipAlpha, v2), v2);
			if (regionCode0 == 0 && regionCode1 == 0)
			{
				innerVertexes.push_back(v1);
			}
			else if (regionCode0 == 0 && regionCode1 != 0)
			{
				innerVertexes.push_back(v2);
			}
			else if (regionCode0 != 0 && regionCode1 == 0)
			{
				innerVertexes.push_back(v2);
				innerVertexes.push_back(v1);
			}
			else
			{
				// exlude
			}
		}
		clipProcess(innerVertexes, ++stage, outputVertexes);
	}

	/*
	* Clip with perspective correct
	* produce vertexes that satisfy :
	*  -w <= x <= w
	*  -w <= y <= w
	*   0 <= z <= w
	*/
	void clipProcessV2(Vertexes & vertexes, int stage, Vertexes & outputVertexes)
	{
		// This function should take 6 pass correspond to the face of clip volume
		if (stage > 6)
		{
			outputVertexes = vertexes;
			return;
		}

		uint8_t regionCode0 = 0, regionCode1 = 0;
		auto begin = vertexes.begin();
		auto end = vertexes.end();
		Vertexes innerVertexes = Vertexes();
		for (auto cur = begin; cur != end; ++cur)
		{
			Vertex &v0 = *cur;
			auto tmpIter = begin;
			if ((cur + 1) != end)
			{
				tmpIter = cur + 1;
			}
			Vertex &v1 = *tmpIter;

			Vector4 &pos0 = v0.position;
			Vector4 &pos1 = v1.position;
			LSFloat clipAlpha = 0.0f;
			LSFloat w0 = (pos0.w);
			LSFloat w1 = (pos1.w);
			LSFloat absw0 = std::fabs(w0);
			LSFloat absw1 = std::fabs(w1);

			// The w component be negative means the point before near plane
			bool w0Valid = pos0.w > 1e-6;
			bool w1Valid = pos1.w > 1e-6;
			bool isWValid = w0Valid || w1Valid;
			// If both w0 and w1 are invalid, exclude them.
			if (!isWValid)
			{
				continue;
			}

			LSFloat x0 = pos0.x; LSFloat x1 = pos1.x;
			LSFloat y0 = pos0.y; LSFloat y1 = pos1.y;
			LSFloat z0 = pos0.z; LSFloat z1 = pos1.z;

			LSFloat a0;
			LSFloat a1;
			LSFloat dz1, dw1, w;
			LSFloat needCalcAlpha = true;

			switch (stage)
			{
			case 1:
				regionCode0 = (x0 <= -absw0 ? leftBitCode : 0);
				regionCode1 = (x1 <= -absw1 ? leftBitCode : 0);
				a0 = x0; a1 = x1;
				w0 = -absw0; w1 = -absw1;
				break;
			case 2:
				regionCode0 = (x0 >= absw0 ? rightBitCode : 0);
				regionCode1 = (x1 >= absw1 ? rightBitCode : 0);
				a0 = x0; a1 = x1;
				break;
			case 3:
				regionCode0 = (y0 <= -absw0 ? bottomBitCode : 0);
				regionCode1 = (y1 <= -absw1 ? bottomBitCode : 0);
				a0 = y0; a1 = y1;
				w0 = -absw0; w1 = -absw1;
				break;
			case 4:
				regionCode0 = (y0 >= absw0 ? topBitCode : 0);
				regionCode1 = (y1 >= absw1 ? topBitCode : 0);
				a0 = y0; a1 = y1;
				break;
			case 5:
				regionCode0 = (z0 <= 0 ? nearBitCode : 0);
				regionCode1 = (z1 <= 0 ? nearBitCode : 0);
				dw1 = w1 - w0;
				dz1 = z1 - z0;
				if (regionCode0 != regionCode1 && std::fabs(dz1) > 1e-6 && std::fabs(dw1) > 1e-6)
				{
					// let z = f(w), if line z = aw + b intersect with near plane
					// forms these equation:
					// z1 = aw1 + b
					// z0 = aw0 + b
					// 0 = aw + b  (z = 0 in near plane)
					w = (w0 * z1 - w1 * z0) / dz1;
					clipAlpha = (w - w0) / dw1;
				}
				needCalcAlpha = false;
				break;
			case 6:
				regionCode0 = (z0 >= absw0 ? topBitCode : 0);
				regionCode1 = (z1 >= absw1 ? topBitCode : 0);
				dw1 = w1 - w0;
				dz1 = z1 - z0;
				if (regionCode0 != regionCode1 && std::fabs(dw1 - dz1) > 1e-6 && std::fabs(dw1) > 1e-6)
				{
					// let z = f(w), if line z = aw + b intersect with far plane
					// forms these equation:
					// z1 = aw1 + b
					// z0 = aw0 + b
					// w = aw + b  (z = w in far plane)
					w = (w1 * z0 - w0 * z1) / (dw1 - dz1);
					clipAlpha = (w - w0) / dw1;
				}
				needCalcAlpha = false;
				break;
			default:
				break;
			}

			LSFloat daw1 = a1 - w1;
			if (needCalcAlpha && regionCode0 != regionCode1 && std::fabs(daw1) > 1e-6)
			{
				// infered by homothetic triangle
				// explain later!
				LSFloat f = (a0 - w0) / -daw1;
				LSFloat w = f * (w1 - w0) / (f + 1.0) + w0;
				clipAlpha = (w - w0) / (w1 - w0);
			}

			Vertex v2 = v0;
			Vertex::add(v0, Vertex::multiply(Vertex::substract(v1, v0, v2), clipAlpha, v2), v2);
			if (regionCode0 == 0 && regionCode1 == 0)
			{
				innerVertexes.push_back(v1);
			}
			else if (regionCode0 == 0 && regionCode1 != 0)
			{
				innerVertexes.push_back(v2);
			}
			else if (regionCode0 != 0 && regionCode1 == 0)
			{
				innerVertexes.push_back(v2);
				innerVertexes.push_back(v1);
			}
			else
			{
				// exlude
			}
		}
		clipProcessV2(innerVertexes, ++stage, outputVertexes);
	}

	void Renderer::clip(Vertexes & vertexes, Vertexes & outputVertexes)
	{
		clipProcessV2(vertexes, 1, outputVertexes);
	}

	bool Renderer::cullFace(const Vertexes & vertexes)
	{
		if (vertexes.size() < 3)
		{
			return true;
		}
		Vector3 n0 = Vector3(vertexes[1].position - vertexes[0].position);
		Vector3 n1 = Vector3(vertexes[2].position - vertexes[1].position);
		n0.z = 0;
		n1.z = 0;
		Vector3 n2 = n0.cross(n1);

		if (frontFace == FaceMode::CCW)
		{
			return n2.z >= 0;
		}
		else
		{
			return n2.z < 0;
		}
	}

	void Renderer::fillPolygon(const Vertexes & vertexes)
	{
		if (vertexes.size() < 3)
		{
			return;
		}

		struct Edge
		{
			Edge(const Point2D& p0, const Point2D& p1, const Vertex & v0, const Vertex & v1)
			{
				if (p0.y <= p1.y)
				{
					pLow = p0; pHigh = p1; vLow = v0; vHigh = v1;
				}
				else
				{
					pLow = p1; pHigh = p0; vLow = v1; vHigh = v0;
				}
				dx = pHigh.x - pLow.x;
				dy = pHigh.y - pLow.y;
			}
			Point2D pLow;
			Point2D pHigh;
			Vertex vLow;
			Vertex vHigh;
			int dx;
			int dy;
		};
		auto beginIter = vertexes.begin();
		auto endIter = vertexes.end();

		std::vector<Point2D> points;
		Vertexes validVertexes;
		for (auto iter = beginIter; iter != endIter; ++iter)
		{
			Point2D p = colorBuffer->normalizeCoordToScreenCoord(iter->position);
			if (points.empty())
			{
				points.push_back(p);
				validVertexes.push_back(*iter);
			}
			if (!points.empty())
			{
				if (!(p == points.back()))
				{
					points.push_back(p);
					validVertexes.push_back(*iter);
				}
				else
				{
					p.x = 0;
				}
			}
		}
		if (validVertexes.size() < 3)
		{
			return;
		}


		typedef std::vector<Edge> Edges;
		Edges edgeVector;
		for (size_t i = 0, size = points.size(); i < size; ++i)
		{
			size_t j = i + 1;
			if (j == size)
			{
				j = 0;
			}
			edgeVector.push_back({ points[i], points[j], validVertexes[i], validVertexes[j] });
		}

		std::map<int, Edges> edgesMap;
		for (const Edge &edge : edgeVector)
		{
			int key = edge.pLow.y;
			if (edgesMap.find(key) == edgesMap.end())
			{
				edgesMap[key] = Edges{ edge };
			}
			else
			{
				edgesMap[key].push_back(edge);
			}
		}

		Edge *prevEedge = nullptr;
		auto cur = edgesMap.begin();
		auto edgesMapEnd = edgesMap.end();
		Vertex	vertexLeft, vertexRight;
		Vertex	vertexTmp1, vertexTmp2;
		//LSFloat nearest = 1 / DEPTH_MAX;
		for (; cur != edgesMapEnd; ++cur)
		{
			Edges &edges = cur->second;
			if (prevEedge)
			{
				edges.push_back(*prevEedge);
				prevEedge = nullptr;
			}
			std::sort(edges.begin(), edges.end(), [](const Edge &lhs, const Edge& rhs)
			{
				if (lhs.pLow.x == rhs.pLow.x)
				{
					if (lhs.dx == 0)
					{
						return rhs.dx > 0;
					}
					else if (rhs.dx == 0)
					{
						return lhs.dx < 0;
					}

					float kLhs = (float)lhs.dy / lhs.dx;
					float kRhs = (float)rhs.dy / rhs.dx;
					if (kLhs * kRhs > 0)
					{
						return kLhs > kRhs;
					}
					else if (kLhs == 0)
					{
						return lhs.dx < 0;
					}
					else if (kRhs == 0)
					{
						return rhs.dx > 0;
					}
					else if (kLhs < 0)
					{
						return true;
					}
					else if (kLhs > 0)
					{
						return false;
					}
					return false;
				}
				else
				{
					return lhs.pLow.x < rhs.pLow.x;
				}
			});

			size_t edgeCount = edges.size();
			Edge &edgeLeft = edges[0];
			int lineY = 0;
			int higherX = 0;
			for (size_t i = 1; i < edgeCount; ++i)
			{
				Edge &edgeRight = edges[i];
				int xleft = edgeLeft.pLow.x;
				// 水平方向多渲染1像素，以免相邻面的重合的两条边出现缝隙
				int xright = edgeRight.pLow.x + 1;
				int countleft = 0, countright = 0;
				int yLimit = 0;
				Edge *higherEdge;
				bool isLeftHigher = (edgeLeft.pHigh.y >= edgeRight.pHigh.y);
				if (isLeftHigher)
				{
					yLimit = edgeRight.pHigh.y;
					higherEdge = &edgeLeft;
				}
				else
				{
					yLimit = edgeLeft.pHigh.y;
					higherEdge = &edgeRight;
				}

				for (int y = edgeLeft.pLow.y; y <= yLimit; ++y)
				{
					lineY = y;
					higherX = isLeftHigher ? xleft : xright;

					float ratioLeft = edgeLeft.dy > 0 ? (float)(y - edgeLeft.pLow.y) / edgeLeft.dy : 0;
					Vertex::multiply(edgeLeft.vLow, (1 - ratioLeft), vertexTmp1);
					Vertex::multiply(edgeLeft.vHigh, ratioLeft, vertexTmp2);
					Vertex::add(vertexTmp1, vertexTmp2, vertexLeft);

					float ratioRight = edgeRight.dy > 0 ? (float)(y - edgeRight.pLow.y) / edgeRight.dy : 0;
					Vertex::multiply(edgeRight.vLow, (1 - ratioRight), vertexTmp1);
					Vertex::multiply(edgeRight.vHigh, ratioRight, vertexTmp2);
					Vertex::add(vertexTmp1, vertexTmp2, vertexRight);

					Vertex finalVertex;
					for (int x = xleft; x <= xright; ++x)
					{
						float ratio = xright > xleft ? (float)(x - xleft) / (xright - xleft) : 0;
						Vertex::multiply(vertexLeft, (1 - ratio), vertexTmp1);
						Vertex::multiply(vertexRight, ratio, vertexTmp2);
						Vertex::add(vertexTmp1, vertexTmp2, finalVertex);

						// depth test
						DepthFormat depth = static_cast<DepthFormat>(finalVertex.position.z * DEPTH_MAX);//finalVertex.position.z > nearest ? UINT64_MAX - (1 / finalVertex.position.z) : 0;
						DepthFormat *existDepthPtr = reinterpret_cast<DepthFormat*>(depthBuffer->getPixelUnit(x, y));
						if (existDepthPtr && depth < *existDepthPtr)
						{
							// write depth
							depthBuffer->setPixelUnit(x, y, reinterpret_cast<uint8_t *>(&depth));
							// write color
							Vector4 color;
							if (shader)
							{
								shader->fragment = &finalVertex;
								color = shader->fragmentShading();
							}
							else
							{
								color = finalVertex.color;
							}
							colorBuffer->setByte(x, y, (uint8_t)(std::fmax(0.0, std::fmin(1.0, color.z)) * 255), 0);
							colorBuffer->setByte(x, y, (uint8_t)(std::fmax(0.0, std::fmin(1.0, color.y)) * 255), 1);
							colorBuffer->setByte(x, y, (uint8_t)(std::fmax(0.0, std::fmin(1.0, color.x)) * 255), 2);
							++setPixelCount;
						}
					}
					countleft += std::abs(edgeLeft.dx);
					if (countleft >= edgeLeft.dy && edgeLeft.dy > 0)
					{
						xleft += (edgeLeft.dx > 0 ? 1 : -1) * countleft / edgeLeft.dy;
						countleft = countleft % edgeLeft.dy;
					}
					countright += std::abs(edgeRight.dx);
					if (countright >= edgeRight.dy && edgeRight.dy > 0)
					{
						xright += (edgeRight.dx > 0 ? 1 : -1) * countright / edgeRight.dy;
						countright = countright % edgeRight.dy;
					}
				}
				edgeLeft = *higherEdge;
			}
			if (edgeLeft.pHigh.y > lineY)
			{
				prevEedge = &edgeLeft;
				float ratio = ((float)lineY - edgeLeft.pLow.y) / (edgeLeft.pHigh.y - edgeLeft.pLow.y);
				Vertex::multiply(prevEedge->vLow, (1 - ratio), vertexTmp1);
				Vertex::multiply(prevEedge->vHigh, ratio, vertexTmp2);
				Vertex::add(vertexTmp1, vertexTmp2, prevEedge->vLow);

				prevEedge->pLow.x = higherX;
				prevEedge->pLow.y = lineY;

				prevEedge->dx = static_cast<int>(prevEedge->dx * (1 - ratio) + 0.5f);
				prevEedge->dy = static_cast<int>(prevEedge->dy * (1 - ratio) + 0.5f);
			}
		}
	}

	void fragmentShading()
	{

	}

	void Renderer::lineTo(const Vertex & p0, const Vertex & p1)
	{
		int x0 = static_cast<int>((p0.position.x + 1) / 2 * viewWidth);
		int y0 = static_cast<int>((p0.position.y + 1) / 2 * viewHeight);
		int x1 = static_cast<int>((p1.position.x + 1) / 2 * viewWidth);
		int y1 = static_cast<int>((p1.position.y + 1) / 2 * viewHeight);

		int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
		int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
		int err = (dx > dy ? dx : -dy) / 2, e2;
		int x = x0, y = y0;
		while (true)
		{
			colorBuffer->setByte(x, y, 0, 0);
			colorBuffer->setByte(x, y, 255, 1);
			colorBuffer->setByte(x, y, 0, 2);
			if (x == x1 && y == y1)
			{
				break;
			}
			e2 = err;
			if (e2 > -dx)
			{
				err -= dy; x += sx;
			}

			if (e2 < dy)
			{
				err += dx; y += sy;
			}
		}
	}

	void Renderer::clearBuffers()
	{
		if (colorBuffer)
		{
			colorBuffer->clear(0);
		}
		if (depthBuffer)
		{
			depthBuffer->clear(0xff);
		}
	}

	RenderBuffer * Renderer::getColorBuffer()
	{
		return colorBuffer;
	}

	RenderBuffer * Renderer::getDepthBuffer()
	{
		return depthBuffer;
	}

	void Renderer::setFrontFace(FaceMode mode)
	{
		frontFace = mode;
	}

	void Renderer::setSampler(LayoutBinding binding, const SamplerPtr& sampler)
	{
		samplers[binding] = sampler;
	}

	SamplerPtr Renderer::getSampler(LayoutBinding binding)
	{
		auto findResult = samplers.find(binding);
		if (findResult == samplers.end())
		{
			return nullptr;
		}
		return findResult->second;
	}

	void Renderer::setShader(const std::shared_ptr<Shader>& inShader)
	{
		shader = inShader;
		shader->setRenderContext(this);
	}

	void Renderer::setDescriptorSet(const std::vector<DescriptorPtr>& descriptors)
	{
		for (DescriptorPtr descriptor : descriptors)
		{
			descriptorSet[descriptor->binding] = descriptor;
		}
	}

	DescriptorPtr Renderer::getDescriptor(const DescriptorBinding& binding)
	{
		auto findResult = descriptorSet.find(binding);
		if (findResult == descriptorSet.end())
		{
			return nullptr;
		}
		return findResult->second;
	}
}