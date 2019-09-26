#include <vector>
#include <string>
#include <chrono>
#include <map>
#include <memory>

#include "Test.h"
#include "Application.h"
#include "Math/Math.h"
#include "Core/Mesh.h"
#include "Shader/Blinn-PhongShader.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <stbimage/stb_image.h>

namespace lsgl
{
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
			moveSpeed *= 0.1f;
			rotateSpeed *= 0.1f;
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
				vertex.normal = { (aiMesh->mNormals + m)->x, (aiMesh->mNormals + m)->y, (aiMesh->mNormals + m)->z };
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
			StaticMeshComponent* childMeshComp = (StaticMeshComponent*)(comp);
			addToMeshComponents(meshCompoents, childMeshComp);
		}
	}

	void Application::init(int width, int height)
	{
		viewWidth = width;
		viewHeight = height;

		renderer = new Renderer(viewWidth, viewHeight);
		renderer->setSampler(0, SamplerPtr(new Sampler(nullptr)));
		renderer->setShader(ShaderPtr(new BlinnPhongShader()));
		StaticMeshComponent* pMeshCompoent = new StaticMeshComponent();

		Assimp::Importer importer;
		std::string modelName = "cyborg";
		std::string modelFilePath = "../Assets/Models/"+ modelName + "/" + modelName + ".obj";
		std::string modelFileBaseDir = "../Assets/Models/" + modelName;
		const aiScene* pScene = importer.ReadFile(modelFilePath, aiProcess_Triangulate | aiProcess_FlipUVs);
		processNode(modelFileBaseDir, pScene, pScene->mRootNode, pMeshCompoent);

		addToMeshComponents(meshComponents, pMeshCompoent);
		
		cameraComponent = new CameraComponent();
		cameraComponent->camera.aspectRatio = (float)viewWidth / viewHeight;
		//cameraComponent->camera.isOrthographic = true;
		//cameraComponent->camera.size = 1000;
		cameraComponent->translate(cameraComponent->getZDirection() * 10);
		cameraComponent->translate(cameraComponent->getYDirection() * 300);
		cameraComponent->rotateY(180);

		uiCameraComponent = new CameraComponent();
		uiCameraComponent->camera.aspectRatio = (float)viewWidth / viewHeight;
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

		std::shared_ptr<Shader::UniformBufferBase> baseUniformBuffer(new Shader::UniformBufferBase());
		baseUniformBuffer->projectMatrix = cameraComponent->getProjectMatrix();
		baseUniformBuffer->viewMatrix = cameraComponent->getViewMatrix();
		DescriptorPtr baseDescriptor(new Descriptor());
		baseDescriptor->binding = "_Base";
		baseDescriptor->stage = PiplineStageBit::Vertex;
		baseDescriptor->data = &(*baseUniformBuffer);

		std::shared_ptr<BlinnPhongShader::UniformBufferApp> appUniformBuffer(new BlinnPhongShader::UniformBufferApp());
		appUniformBuffer->viewPos = cameraComponent->getWorldPosition();
		DescriptorPtr appDescriptor(new Descriptor());
		appDescriptor->binding = "_App";
		appDescriptor->stage = PiplineStageBit::Fragment;
		appDescriptor->data = &(*appUniformBuffer);

		for (auto pMeshComp : meshComponents)
		{
			Matrix4x4 M = pMeshComp->getLocalToWorldTransformMatrix() * Matrix4x4::scale(Vector3::one * WORLD_UNIT);
			baseUniformBuffer->modelMatrix = M;
			baseUniformBuffer->mvpMatrix = P * V * M;
			for (auto pMesh : pMeshComp->meshes)
			{
				Texture* pTexture = pMesh->material.getTexture(MaterialTextureType::Diffuse, 0);
				Sampler sampler = Sampler(pTexture);

				DescriptorPtr diffuseTextureDescriptor(new Descriptor());
				diffuseTextureDescriptor->binding = "_DiffuseTex";
				diffuseTextureDescriptor->stage = PiplineStageBit::Fragment;
				diffuseTextureDescriptor->data = &sampler;

				Texture* pSpecularTexture = pMesh->material.getTexture(MaterialTextureType::Specular, 0);
				Sampler specularSampler = Sampler(pSpecularTexture);
				DescriptorPtr specularTextureDescriptor(new Descriptor());
				specularTextureDescriptor->binding = "_SpecularTex";
				specularTextureDescriptor->stage = PiplineStageBit::Fragment;
				specularTextureDescriptor->data = &specularSampler;

				renderer->setDescriptorSet({ baseDescriptor, appDescriptor, diffuseTextureDescriptor, specularTextureDescriptor });
				renderer->renderVertices(pMesh->vertices, renderMode);
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

		float curTime = (float)clock() / CLOCKS_PER_SEC;
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