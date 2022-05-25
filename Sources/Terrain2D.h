#pragma once

#include "framework.h"
#include "Renderer.h"
#include "NavMesh.h"

using namespace RoninEngine::AIPathFinder;
namespace RoninEngine::Runtime {

	class Terrain2D : public Renderer
	{
		NavMesh* nav;
	public:

		Terrain2D();
		Terrain2D(int width, int length);
		~Terrain2D();

		NavMesh* Get_NavMesh();
		const bool Set_Destination(const Transform* transform, const Vec2 destination, const float speed);

		const bool isCollider(const Vec2 destination);

		Vec2 GetSize();
		void Render(Render_info* render_info);
	};
}

