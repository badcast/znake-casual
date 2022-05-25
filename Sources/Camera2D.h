#pragma once
#include "Camera.h"

namespace RoninEngine::Runtime {
   class Camera2D final : public Camera {
      protected:
         void render(SDL_Renderer* renderer, Rect_t rect, GameObject* root);

      public:
         bool visibleBorders;
         bool visibleGrids;

         Camera2D();
         Camera2D(const string& name) : Camera(name){}
         Camera2D(const Camera2D&);
   };
}  // namespace RoninEngine::Runtime
