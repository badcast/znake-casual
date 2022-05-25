#include "pch.h"
#include "Terrain2D.h"

namespace RoninEngine::Runtime {

	Terrain2D::Terrain2D() : Terrain2D(100, 100) {}
	Terrain2D::Terrain2D(int width, int length) : Renderer("Terrain 2D") {
        GC::gc_alloc_lval(nav, width, length);
	}
    Terrain2D::~Terrain2D() {}
	
	const bool Terrain2D::isCollider(const Vec2 destination) {
		auto n = this->nav->neuron(destination);
		return n || n->locked();
	}


	Vec2 Terrain2D::GetSize() {
		return {};
	}

	void Terrain2D::Render(Render_info* render_info) {

	}
}
