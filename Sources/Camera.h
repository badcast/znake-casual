#pragma once

#include "Object.h"
#include "Application.h"
#include "Scene.h"
#include "Light.h"

namespace RoninEngine::Runtime
{
	class Camera :
		public Component
	{
		friend Level;

	protected:
		bool targetClear;
		list<Renderer*> __rendererOutResults;
		list<Light*> __lightsOutResults;
		virtual void render(SDL_Renderer* renderer, Rect_t rect, GameObject* root) = 0;
	public:
		Vec2 aspectRatio;
        bool enabled;

		Camera();
		Camera(const string& name);
		virtual ~Camera();


		bool isFocused();
		void Focus();

		const Vec2 ScreenToWorldPoint(Vec2 screenPoint);
		const Vec2 WorldToScreenPoint(Vec2 worldPoint);

		std::tuple< list<Renderer*>*, list<Light*>*> GetRenderersOfScreen();

		static Camera* mainCamera();
	};


}
