#include <algorithm>
#include <map>
#include "Renderer.h"
#include <ctime>

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

	void Renderer::renderVertices(const Vertexes &vertices, const Matrix4x4& M, const Matrix4x4& V, const Matrix4x4& P, RenderMode renderMode)
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

			renderTriangle(vertices[i], vertices[i + 1], vertices[i + 2], M, V, P, renderMode);
		}
		clock_t endTime = clock();
		double deltaTime = (double)(endTime - beginTime) * 1000 / CLOCKS_PER_SEC;
		//printf2("render use %f ms\n", deltaTime);
	}

	void Renderer::renderTriangle(const Vertex & p0, const Vertex & p1, const Vertex & p2, const Matrix4x4& M, const Matrix4x4& V, const Matrix4x4& P, RenderMode renderMode)
	{
		Vertex np0 = p0, np1 = p1, np2 = p2;
		np0.position = (P*V*M)*p0.position;
		np1.position = (P*V*M)*p1.position;
		np2.position = (P*V*M)*p2.position;
		Vertexes vertexesInput = { np0, np1, np2 };
		Vertexes vertexes;
		clip(vertexesInput, vertexes);
		// perspective divide
		for (Vertex &vertex : vertexes)
		{
			vertex.position = vertex.position / vertex.position.w;
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
			float clipAlpha = 0;
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
			double x0 = sign0 * pos0.x;
			double x1 = sign1 * pos1.x;
			double y0 = sign0 * pos0.y;
			double y1 = sign1 * pos1.y;
			double z0 = sign0 * pos0.z;
			double z1 = sign1 * pos1.z;

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
				if (regionCode0 ^ regionCode1 != 0)
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
		//regionCode |= sign * pos.x < -absw ? leftBitCode : 0;
		//regionCode |= sign * pos.x > absw ? rightBitCode : 0;
		//regionCode |= sign * pos.y < -absw ? bottomBitCode : 0;
		//regionCode |= sign * pos.y > absw ? topBitCode : 0;
		//regionCode |= sign * pos.z < -absw ? nearBitCode : 0;
		//regionCode |= sign * pos.z > absw ? farBitCode : 0;
	}

	void Renderer::clip(Vertexes & vertexes, Vertexes & outputVertexes)
	{
		clipProcess(vertexes, 1, outputVertexes);
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
		Edges edges;
		for (int i = 0, size = points.size(); i < size; ++i)
		{
			int j = i + 1;
			if (j == size)
			{
				j = 0;
			}
			edges.push_back({ points[i], points[j], validVertexes[i], validVertexes[j] });
		}

		std::map<int, Edges> edgesMap;
		for (const Edge &edge : edges)
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
		int count = 0;
		Vertex	vertexLeft, vertexRight;
		Vertex	vertexTmp1, vertexTmp2;
		double nearest = 1 / DEPTH_MAX;
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

			int edgeCount = edges.size();
			Edge &edgeLeft = edges[0];
			int lineY = 0;
			int higherX = 0;
			for (int i = 1; i < edgeCount; ++i)
			{
				Edge &edgeRight = edges[i];
				int xleft = edgeLeft.pLow.x;
				// ˮƽ�������Ⱦ1���أ�������������غϵ������߳��ַ�϶
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
						DepthFormat depth = finalVertex.position.z * DEPTH_MAX;//finalVertex.position.z > nearest ? UINT64_MAX - (1 / finalVertex.position.z) : 0;
						DepthFormat *existDepthPtr = reinterpret_cast<DepthFormat*>(depthBuffer->getPixelUnit(x, y));
						if (existDepthPtr && depth < *existDepthPtr)
						{
							// write depth
							depthBuffer->setPixelUnit(x, y, reinterpret_cast<uint8_t *>(&depth));
							// write color
							Vector4 color;
							Sampler* pSampler = getSampler(LayoutBinding::DiffuseTexture);
							if (pSampler)
							{
								color = pSampler->getSample(finalVertex.uv.x, finalVertex.uv.y);
							}
							else
							{
								color = finalVertex.color;
							}
							colorBuffer->setByte(x, y, (uint8_t)(color.z * 255), 0);
							colorBuffer->setByte(x, y, (uint8_t)(color.y * 255), 1);
							colorBuffer->setByte(x, y, (uint8_t)(color.x * 255), 2);
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

				prevEedge->dx = prevEedge->dx * (1 - ratio) + 0.5;
				prevEedge->dy = prevEedge->dy * (1 - ratio) + 0.5;
			}
		}
	}

	void fragmentShading()
	{

	}

	void Renderer::lineTo(const Vertex & p0, const Vertex & p1)
	{
		int x0 = (p0.position.x + 1) / 2 * viewWidth;
		int y0 = (p0.position.y + 1) / 2 * viewHeight;
		int x1 = (p1.position.x + 1) / 2 * viewWidth;
		int y1 = (p1.position.y + 1) / 2 * viewHeight;

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

	void Renderer::setSampler(LayoutBinding binding, Sampler * sampler)
	{
		samplers[static_cast<uint32_t>(binding)] = sampler;
	}

	Sampler* Renderer::getSampler(LayoutBinding binding)
	{
		auto findResult = samplers.find(static_cast<uint32_t>(binding));
		if (findResult == samplers.end())
		{
			return nullptr;
		}
		return findResult->second;
	}

	void Renderer::setShader(Shader * inShader)
	{
		shader = inShader;
		shader->setRenderContext(this);
	}
}