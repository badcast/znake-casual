#include "framework.h"

RoninEngine::Runtime::Camera* _main = nullptr;
namespace RoninEngine::Runtime {

Camera::Camera() : Camera("Camera") {}
Camera::Camera(const string& name) : Component(name) {
    if (!_main) {
        _main = this;
    }
    aspectRatio = Vec2::one;
    targetClear = true;
    enabled = true;
}
Camera::~Camera() {
    if (_main == this) _main = nullptr;
}

bool Camera::isFocused() { return _main == this; }

void Camera::Focus() { _main = this; }

/*
std::tuple<list<Renderer*>*, list<Light*>*> linearSelection() {
    constexpr std::uint8_t Nz = 2;
    list<Renderer*> layers[Nz];
    std::uint8_t zN = 0;
    if (__rendererOutResults.empty()) {
        auto res = Application::getResolution();
        Vec2 topLeft, rightBottom, rSz;
        topLeft = this->ScreenToWorldPoint(Vec2::zero);
        rightBottom = this->ScreenToWorldPoint(Vec2(res.width, res.height));

        for (auto render : RoninEngine::Level::getScene()->_assoc_renderers) {
            if (render->zOrder >= Nz) throw std::out_of_range("N z range");

            rSz = render->GetSize() / squarePerPixels / 2;

            Transform* t = render->transform();
            if (render->zOrder >= 0 &&
                //	X
                (t->_p.x + rSz.x >= topLeft.x && t->_p.x - rSz.x <= rightBottom.x) &&
                //	Y
                (t->_p.y - rSz.y <= topLeft.y && t->_p.y + rSz.y >= rightBottom.y)) {
                layers[render->zOrder].emplace_front(render);
            }
        }

            // ordering and collect
            list<Renderer*>* target;
        while ((target = zN < Nz ? &layers[zN++] : nullptr)) {
            for (auto x = begin(*target); x != end(*target); ++x) {
                __rendererOutResults.emplace_back((*x));
            }
        }
    }

    if (__lightsOutResults.empty()) {
        __lightsOutResults.assign(RoninEngine::Level::getScene()->_assoc_lightings.begin(),
                                  RoninEngine::Level::getScene()->_assoc_lightings.end());
    }

    return make_tuple(&__rendererOutResults, &__lightsOutResults);
}
*/

std::tuple<list<Renderer*>*, list<Light*>*> Camera::matrixSelection() {
    constexpr std::uint8_t Nz = 2;
    list<Renderer*> layers[Nz];
    std::uint8_t zN = 0;

    if (__rendererOutResults.empty()) {
        Resolution res = Application::getResolution();
std::unordered_map<int,int> xxx;
        Vec2 ray;
        Vec2 wpLeftTop(Vec2::Round(this->ScreenToWorldPoint(Vec2::zero)));
        Vec2 wpRightBottom(Vec2::Round(this->ScreenToWorldPoint(Vec2(res.width, res.height))));
        float delayTime = Time::startUpTime();
        for (ray.x = wpLeftTop.x; ray.x <= wpRightBottom.x; ++ray.x) {
            for (ray.y = wpLeftTop.y; ray.y >= wpRightBottom.y; --ray.y) {
                auto ifine = Level::getScene()->matrixWorld.find(ray);
                if (ifine != std::end(Level::getScene()->matrixWorld)) {
                    for (auto el : ifine->second) {
                        if (Renderer* render = el->gameObject()->Get_Component<Renderer>()) {
                            layers[render->zOrder].emplace_front(render);
                        }
                    }
                }
            }
        }
        delayTime = Time::startUpTime() - delayTime;
        // ordering and collect
        list<Renderer*>* target;
        while ((target = zN < Nz ? &layers[zN++] : nullptr)) {
            for (auto x = begin(*target); x != end(*target); ++x) {
                __rendererOutResults.emplace_back((*x));
            }
        }
    }

    if (__lightsOutResults.empty()) {
        __lightsOutResults.assign(RoninEngine::Level::getScene()->_assoc_lightings.begin(),
                                  RoninEngine::Level::getScene()->_assoc_lightings.end());
    }

    return make_tuple(&__rendererOutResults, &__lightsOutResults);
}

const Vec2 Camera::ScreenToWorldPoint(Vec2 screenPoint) {
    Resolution res = Application::getResolution();
    Vec2 offset = _main->transform()->position();
    screenPoint.x = (res.width / 2.f - (screenPoint.x)) * -1;
    screenPoint.y = res.height / 2.f - (screenPoint.y);
    screenPoint /= squarePerPixels;
    screenPoint += offset;
    return screenPoint;
}
const Vec2 Camera::WorldToScreenPoint(Vec2 worldPoint) {
    Resolution res = Application::getResolution();
    //Положение по горизонтале
    worldPoint.x = ((res.width) / 2.0f - (_main->transform()->_p.x - worldPoint.x) * squarePerPixels);
    //Положение по вертикале
    worldPoint.y = ((res.height) / 2.0f + (_main->transform()->_p.y - worldPoint.y) * squarePerPixels);
    return worldPoint;
}
Camera* Camera::mainCamera() { return _main; }

}  // namespace RoninEngine::Runtime
