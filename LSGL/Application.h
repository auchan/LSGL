#pragma once
#include <cstdint>
#include <ctime>
#include <set>
#include "Core/World.h"
#include "Core/Renderer.h"
#include "Core/Camera.h"
#include "Components/CameraComponent.h"
#include "Components/StaticMeshComponent.h"

namespace lsgl
{
	enum Key
	{
		SHIFT = 0x10,
		SPACE = 0x20,

		LEFT = 0x25,
		UP = 0x26,
		RIGHT = 0x27,
		DOWN = 0x28,
	};

	class Application
	{
	public:
		static Application& instance()
		{
			static Application instance;
			return instance;
		}
		~Application();
	public:
		void init(int width, int height);
		void update();
		void postUpdate();
		void quit();

		void onKeyDown(int key);
		void onKeyUp(int key);

		bool isKeyDown(int key);

		RenderBuffer* getColorBuffer();
	private:

		Application();
		void updateKeys();
		void draw();

		Renderer* renderer;
		CameraComponent *cameraComponent;
		CameraComponent *uiCameraComponent;

		std::vector<StaticMeshComponent*> meshComponents;

		World* world = nullptr;
		int viewWidth;
		int viewHeight;

		float deltaTime = 0;
		float lastUpdateTime = 0;

		std::set<int> keyDownSet;
		RenderMode renderMode = RenderMode::Wireframe;
	};
}

