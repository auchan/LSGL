#include <vector>
#include "Application.h"
#include "Math/Math.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Test.h"
#include "Core/Mesh.h"
#include <string>
#include <chrono>
#include <map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <stbimage/stb_image.h>

namespace lsgl
{
	union MyUnion
	{
		int i;
		void* p;
	};

	RenderBuffer* Application::getColorBuffer()
	{
		if (renderer == nullptr)
		{
			return nullptr;
		}
		return renderer->getColorBuffer();
	}

	Application::Application()
		:renderer(nullptr)
	{
		
	}

	Application::~Application()
	{
		delete renderer;
		delete cameraComponent;
		delete uiCameraComponent;
		delete world;

		for (StaticMeshComponent* meshComponent : meshComponents)
		{
			delete meshComponent;
		}
	}

	void Application::quit()
	{
	}

	void Application::onKeyDown(int key)
	{
		keyDownSet.insert(key);

		if (isKeyDown(Key::SPACE))
		{
			if (renderMode + 1 == RenderMode::Count)
			{
				renderMode = (RenderMode)0;
			}
			else
			{
				renderMode = (RenderMode)((int)renderMode + 1);
			}
		}
	}

	void Application::onKeyUp(int key)
	{
		keyDownSet.erase(key);
	}

	bool Application::isKeyDown(int key)
	{
		return keyDownSet.find(key) != keyDownSet.end();
	}

	void Application::updateKeys()
	{
		float moveSpeed = 100;
		float rotateSpeed = 30;
		if (isKeyDown(Key::SHIFT))
		{
			moveSpeed *= 0.1;
			rotateSpeed *= 0.1;
		}

		float deltaMove = deltaTime * moveSpeed;
		float deltaRotate = deltaTime * rotateSpeed;

		Vector3 zDirection = cameraComponent->getZDirection();
		Vector3 xDirection = cameraComponent->getXDirection();

		if (isKeyDown('W'))
		{
			cameraComponent->translate(zDirection * deltaMove);
		}
		if (isKeyDown('S'))
		{
			cameraComponent->translate(zDirection * -deltaMove);
		}
		if (isKeyDown('A'))
		{
			cameraComponent->translate(xDirection * -deltaMove);
		}
		if (isKeyDown('D'))
		{
			cameraComponent->translate(xDirection * deltaMove);
		}
		if (isKeyDown('Q'))
		{
			cameraComponent->translate(Vector3(0, -deltaMove, 0));
		}
		if (isKeyDown('E'))
		{
			cameraComponent->translate(Vector3(0, deltaMove, 0));
		}
		if (isKeyDown(Key::LEFT))
		{
			cameraComponent->rotateY(-deltaRotate);
		}
		if (isKeyDown(Key::RIGHT))
		{
			cameraComponent->rotateY(deltaRotate);
		}
		if (isKeyDown(Key::UP))
		{
			cameraComponent->rotateX(-deltaRotate);
		}
		if (isKeyDown(Key::DOWN))
		{
			cameraComponent->rotateX(deltaRotate);
		}
	}

	void processNode(const std::string& modelFileBaseDir, const aiScene* pScene, const aiNode* pNode, StaticMeshComponent* staticMesh)
	{
		aiVector3D scaling, position;
		aiQuaternion rotation;
		pNode->mTransformation.Decompose(scaling, rotation, position);
		staticMesh->setLocalPosition({ position.x, position.y, position.z });
		staticMesh->setLocalScale({ scaling.x, scaling.y, scaling.z });

		staticMesh->meshes.reserve(pNode->mNumMeshes);
		for (unsigned int i = 0; i < pNode->mNumMeshes; ++i)
		{
			aiMesh* aiMesh = pScene->mMeshes[pNode->mMeshes[i]];
			Mesh* pMesh = new Mesh();
			staticMesh->meshes.push_back(pMesh);
			for (unsigned int m = 0; m < aiMesh->mNumVertices; ++m)
			{
				aiVector3D &aiVertex = aiMesh->mVertices[m];
				Vertex vertex = { {aiVertex.x, aiVertex.y, aiVertex.z, 1.0}, {0.0f, 1.0f, 0.0f, 1.0f} };
				vertex.uv = { (aiMesh->mTextureCoords[0] + m)->x, (aiMesh->mTextureCoords[0] + m)->y };
				pMesh->vertices.push_back(vertex);
			}

			pMesh->material.modelFileBaseDir = modelFileBaseDir;
			pMesh->material.rawMaterial = new aiMaterial();
			aiMaterial *aiMaterial = pScene->mMaterials[aiMesh->mMaterialIndex];
			aiMaterial::CopyPropertyList(pMesh->material.rawMaterial, aiMaterial);

			int shadingModel;
			aiMaterial->Get(AI_MATKEY_SHADING_MODEL, shadingModel);
		}

		// process children
		for (unsigned int i = 0; i < pNode->mNumChildren; ++i)
		{
			StaticMeshComponent* childComponent = new StaticMeshComponent();
			staticMesh->addChild(*childComponent);
			processNode(modelFileBaseDir, pScene, pNode->mChildren[i], childComponent);
		}
	}

	void addToMeshComponents(std::vector<StaticMeshComponent*> &meshCompoents, StaticMeshComponent* meshComp)
	{
		meshCompoents.push_back(meshComp);
		for (auto comp : meshComp->getChildren())
		{
			StaticMeshComponent* meshComp = (StaticMeshComponent*)(comp);
			addToMeshComponents(meshCompoents, meshComp);
		}
	}

	void Application::init(int width, int height)
	{

		viewWidth = width;
		viewHeight = height;

		//Vector4 test1 = Matrix4x4::rotateY(-45) * Matrix4x4::rotateX(45) * Vector4(1, 0, 0, 1);
		//Vector4 test2 = Matrix4x4::rotateZ(45) * Matrix4x4::rotateX(-45) * Matrix4x4::rotateZ(0) * Vector4(1, 0, 0, 1);
		//Vector4 test3 = Matrix4x4::rotateEuler(Vector3(45, -45, 0)) * Vector4(1, 0, 0, 1);
		//std::cout << test1 << std::endl;

		//SceneComponent s1 = SceneComponent();
		//s1.setLocalPosition(Vector3(100, 100, 100));
		//s1.setLocalScale(Vector3(2, 4, 1));
		//s1.setLocalRotation(Vector3(0, 0, 90));

		//SceneComponent s2 = SceneComponent();
		//s2.setLocalPosition(Vector3(0, 100, 0));
		//s2.setLocalRotation(Vector3(0, 0, -180));
		//s2.attachTo(s1);
		//Vector3 wps = s2.getWorldPosition();

		//SceneComponent s3 = SceneComponent();
		//s3.setLocalPosition(Vector3(100, 0, 0));
		//s3.attachTo(s2);
		//Vector3 wps2 = s3.getWorldPosition();
		renderer = new Renderer(viewWidth, viewHeight);
		renderer->setSampler(LayoutBinding::DiffuseTexture, new Sampler(nullptr));

		StaticMeshComponent* pMeshCompoent = new StaticMeshComponent();

		Assimp::Importer importer;
		std::string modelFilePath = "../Assets/Models/nanosuit/nanosuit.obj";
		std::string modelFileBaseDir = "../Assets/Models/nanosuit";
		const aiScene* pScene = importer.ReadFile(modelFilePath, aiProcess_Triangulate | aiProcess_FlipUVs);
		processNode(modelFileBaseDir, pScene, pScene->mRootNode, pMeshCompoent);

		//renderer->pSampler = new Sampler(mesh.pDiffuseTexture);

		//mesh.vertices = { Vertexes {VERTICES_QUAD} };
		addToMeshComponents(meshComponents, pMeshCompoent);
		
		//meshs.push_back({ Vertexes {VERTICES_CUBE} });
		//meshs.push_back({ Vertexes {VERTICES_CUBE} });
		//mesh = new Mesh();
		//mesh->vertices = ;

		cameraComponent = new CameraComponent();
		cameraComponent->camera.aspectRatio = viewWidth / viewHeight;
		//cameraComponent->camera.isOrthographic = true;
		//cameraComponent->camera.size = 1000;
		cameraComponent->translate(cameraComponent->getZDirection() * 10);
		cameraComponent->translate(cameraComponent->getYDirection() * 300);
		cameraComponent->rotateY(180);

		uiCameraComponent = new CameraComponent();
		uiCameraComponent->camera.aspectRatio = viewWidth / viewHeight;
		uiCameraComponent->camera.isOrthographic = true;
		uiCameraComponent->camera.size = 200;

		world = new World();
		world->init();

		lastUpdateTime = 0;
		deltaTime = 0;
	}

	void Application::draw()
	{
		if (!renderer)
		{
			return;
		}

		Matrix4x4 V, P;
		V = cameraComponent->getViewMatrix();
		P = cameraComponent->getProjectMatrix();
		for (auto pMeshComp : meshComponents)
		{
			Matrix4x4 M = pMeshComp->getLocalToWorldTransformMatrix() * Matrix4x4::scale(Vector3::one * WORLD_UNIT);
			for (auto pMesh : pMeshComp->meshes)
			{
				Texture* pTexture = pMesh->material.getTexture(MaterialTextureType::Diffuse, 0);
				renderer->getSampler(LayoutBinding::DiffuseTexture)->setTexture(pTexture);
				renderer->renderVertices(pMesh->vertices, M, V, P, renderMode);
			}
		}

		////render ui
		//V = uiCameraComponent->getViewMatrix();
		//P = uiCameraComponent->getProjectMatrix();
		//i = 0;
		//for (auto mesh : meshs)
		//{
		//	M = Matrix4x4::translate({ LSFloat(i * 200), 0, 200 }) * Matrix4x4::scale(Vector3::one);
		//	renderer->render(mesh, M, V, P, renderMode);
		//	++i;
		//}
	}

	void Application::update()
	{
		if (world == nullptr)
		{
			return;
		}

		updateKeys();

		double curTime = (double)clock() / CLOCKS_PER_SEC;
		deltaTime = curTime - lastUpdateTime;
		world->update(deltaTime);
		printf2("fps: %.2f\n", 1 / deltaTime);
		
		draw();

		lastUpdateTime = curTime;
	}

	void Application::postUpdate()
	{
		if (renderer)
		{
			renderer->clearBuffers();
		}
	}

}