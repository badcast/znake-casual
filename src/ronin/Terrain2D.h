#pragma once

#include "Renderer.h"
#include "dependency.h"

namespace RoninEngine::Runtime {

class Terrain2D : public Renderer {
    RoninEngine::AIPathFinder::NavMesh* nav;

   public:
    Terrain2D();
    Terrain2D(int width, int length);
    ~Terrain2D();

    RoninEngine::AIPathFinder::NavMesh* getNavMesh();
    const bool Set_Destination(const Transform* transform, const Vec2 destination, const float speed);

    const bool isCollider(const Vec2 destination);

    Vec2 GetSize() override;
    void Render(Render_info* render_info);
};
}  // namespace RoninEngine::Runtime
