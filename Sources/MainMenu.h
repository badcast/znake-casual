#pragma once
#include "framework.h"
#include "Scene.h"

namespace RoninEngine {

	class MainMenu : public Level
	{
	public:
        MainMenu() : Level("Main Menu") {}

		void awake();
		void start();
		void update();
		void lateUpdate();
		void onDrawGizmos();
		void onUnloading();

	};
}

