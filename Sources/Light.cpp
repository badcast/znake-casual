#include "Light.h"
#include "inputSystem.h"
#include "pch.h"

namespace RoninEngine::Runtime {
Light::Light() : Light(typeid(Light).name()) {}
Light::Light(const string &name) : Component(name) { fieldFogTexture = nullptr; }

Light::~Light() {}

void Light::GetLightSource(Render_info *render) {
  auto display = Application::display();
  Color c;
  SDL_Texture *target, *lastTarget;

  if (!fieldFogTexture) {
    GC::gc_alloc_texture(&fieldFogTexture, display.w, display.h,
                         SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING);
    fieldFogTexture->blendMode(SDL_BlendMode::SDL_BLENDMODE_BLEND);

    c = fieldFogTexture->color();
    c.r = 0;
    c.g = 0;
    c.b = 0;
    c.a = 255;

    fieldFogTexture->color(c);
  }

  lastTarget = SDL_GetRenderTarget(render->renderer);

  target = SDL_CreateTexture(render->renderer, SDL_PIXELFORMAT_RGBA8888,
                             SDL_TextureAccess::SDL_TEXTUREACCESS_TARGET,
                             display.w, display.h);

  SDL_SetRenderTarget(render->renderer, target);
  SDL_SetRenderDrawColor(render->renderer, 0, 0, 0, 55);
  //SDL_RenderClear(render->renderer);

  // Get light
  this->GetLight(render->renderer);

  SDL_SetRenderTarget(render->renderer, lastTarget);
  SDL_RenderCopy(render->renderer, target, nullptr, nullptr); // copy
  SDL_DestroyTexture(target);
}
} // namespace RoninEngine::Runtime
