#pragma once

#include "dependency.h"
#include "Renderer.h"

namespace RoninEngine::Runtime {

     class Terrain2D : public Renderer
	{
          RoninEngine::AIPathFinder::NavMesh* nav;
	public:

		Terrain2D();
		Terrain2D(int width, int length);
		~Terrain2D();

          RoninEngine::AIPathFinder::NavMesh* Get_NavMesh();
		const bool Set_Destination(const Transform* transform, const Vec2 destination, const float speed);

		const bool isCollider(const Vec2 destination);

		Vec2 GetSize();
		void Render(Render_info* render_info);
	};
}

