#pragma once
#include "dependency.h"

namespace RoninEngine::Runtime {
class Gizmos {
   public:
    static Color color;
    static float angle;

    static void DrawLine(Vec2 a, Vec2 b);
    static void DrawPosition(const Vec2& origin, float scalar = maxWorldScalar);
    static void Draw2DWorldSpace(const Vec2& origin, int depth = 32);
    static void DrawNavMesh(AIPathFinder::NavMesh* map);
    static void DrawTriangle(Vec2 pos, float base, float height);
    static void DrawTextOnPosition(Vec2 origin, const std::string& text);

    static float square(float x);
    static float square_rectangle(float a, float b);
    static float square_rectangle(Vec2 p1, Vec2 p2, Vec2 p3, Vec2 p4);
    static float square_circle(float radius);
    static float square_triangle(float base, float height);
    static float square_triangle(Vec2 p1, Vec2 p2, Vec2 p3);
    static float square_mesh(std::list<Vec2>&& vec);
};
}  // namespace RoninEngine::Runtime
