#pragma once

#include "framework.h"
#include "Component.h"
#include "Renderer.h"

namespace RoninEngine::Runtime
{
	class Light : public Component
	{
        Texture* fieldFogTexture;
	protected:
		virtual void GetLight(SDL_Renderer* renderer) = 0;
	public:
		Light();
		Light(const string& name);
		virtual ~Light();
		void GetLightSource(Render_info* render_info);
	};
}

