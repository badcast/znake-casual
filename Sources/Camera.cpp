#include "pch.h"
#include "Camera.h"
#include "Renderer.h"


RoninEngine::Runtime::Camera* _main = nullptr;
namespace RoninEngine::Runtime
{
	Camera::Camera() : Camera("Camera") {
	}
	Camera::Camera(const string& name) : Component(name) {
		if (!_main) {
			_main = this;
		}
		aspectRatio = Vec2::one;
		targetClear = true;
        enabled = true;
	}
	Camera::~Camera() {
		if (_main == this)
			_main = nullptr;
	}
	bool Camera::isFocused() {
		return _main == this;
	}
	void Camera::Focus() {
		_main = this;
	}
	std::tuple< list<Renderer*>*, list<Light*>*> Camera::GetRenderersOfScreen() {
		constexpr int Nz = 5;
		list<Renderer*> renderers[Nz];
		list<Renderer*>* target = &renderers[0];
		int zN = Nz;
        int size = 0;

		if (__rendererOutResults.empty())
		{
            auto display = Application::display();
			Vec2 topLeft, rightBottom, rSz;
			topLeft = this->ScreenToWorldPoint(Vec2::zero);
			rightBottom = this->ScreenToWorldPoint(Vec2(display.w, display.h));

            //todo: collect
			for (auto render : Level::getScene()->_assoc_renderers)
			{
				if (render->zOrder >= Nz)
					throw std::out_of_range("N z range");

				rSz = render->GetSize() / squarePerPixels / 2;

				Transform* t = render->transform();
				if (render->zOrder >= 0 &&
					//	X											
                         (t->_p.x + rSz.x >= topLeft.x && t->_p.x - rSz.x <= rightBottom.x) &&
					//	Y
                         (t->_p.y - rSz.y <= topLeft.y && t->_p.y + rSz.y >= rightBottom.y)) {
					renderers[render->zOrder].emplace_front(render);
				}
			}

			//ordering and collect
			target = &renderers[--zN];
			while (target)
			{
				for (auto i = begin(*target); i != end(*target); ++i)
				{
					__rendererOutResults.emplace_front((*i));
				}

				if (zN)
					target = &renderers[--zN];
				else
                    target = nullptr;
			}
		}

		if (__lightsOutResults.empty())
		{
			__lightsOutResults.assign(Level::getScene()->_assoc_lightings.begin(), Level::getScene()->_assoc_lightings.end());
		}

		return make_tuple(&__rendererOutResults, &__lightsOutResults);
	}
	const Vec2 Camera::ScreenToWorldPoint(Vec2 screenPoint) {
		Vec2 lhs = transform()->position();

		//dst.x = ((rect.w - dst.w) / 2.0f - (point->x + sourcePoint->x) * squarePerPixels);
		//dst.y = ((rect.h - dst.h) / 2.0f + (point->y - sourcePoint->y) * squarePerPixels);

        auto display = Application::display();
		screenPoint.x = display.w / 2.f - (screenPoint.x);
		screenPoint.x *= -1;
		screenPoint.y = display.h / 2.f - (screenPoint.y);
		screenPoint /= squarePerPixels;
		screenPoint += lhs;
		return screenPoint;
	}
	const Vec2 Camera::WorldToScreenPoint(Vec2 worldPoint) {
          Vec2* point = &transform()->_p;
		Vec2 dst;
        auto display = Application::display();
		//Положение по горизонтале 
		dst.x = ((display.w ) / 2.0f - (point->x - worldPoint.x) * squarePerPixels);
		//Положение по вертикале 
		dst.y = ((display.h) / 2.0f + (point->y - worldPoint.y) * squarePerPixels);
		return dst;
	}
	Camera* Camera::mainCamera()
	{
		return _main;
	}

}
