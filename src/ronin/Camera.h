#pragma once

#include "Component.h"
#include "dependency.h"

namespace RoninEngine::Runtime {
class Camera : public Component {
    friend RoninEngine::Level;

   protected:
    bool targetClear;
    std::list<Renderer*> __rendererOutResults;
    std::list<Light*> __lightsOutResults;
    virtual void render(SDL_Renderer* renderer, Rect_t rect, GameObject* root) = 0;

   public:
    Vec2 aspectRatio;
    bool enabled;

    Camera();
    Camera(const std::string& name);

    virtual ~Camera();

    bool isFocused();
    void Focus();

    const Vec2 ScreenToWorldPoint(Vec2 screenPoint);
    const Vec2 WorldToScreenPoint(Vec2 worldPoint);

    std::tuple<std::list<Renderer*>*, std::list<Light*>*> GetRenderersOfScreen();

    static Camera* mainCamera();
};

}  // namespace RoninEngine::Runtime
