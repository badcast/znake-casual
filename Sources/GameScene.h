#pragma once
#include "framework.h"
#include "Scene.h"

namespace RoninEngine
{
	class GameScene :
		public Level
	{
	public:
		GameScene() : Level("Game Scene levels[XXX]") {

		}
		void awake() override;
		void start() override;
		void update() override;
		void lateUpdate() override;
		void onDrawGizmos() override;
		void onUnloading() override;
	};
}
