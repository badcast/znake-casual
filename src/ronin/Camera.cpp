#include "framework.h"

RoninEngine::Runtime::Camera* _main = nullptr;
namespace RoninEngine::Runtime {

Camera::Camera() : Camera(typeid(*this).name()) {}
Camera::Camera(const std::string& name) : Component(name), aspectRatio(Vec2::one) {
    if (!_main) {
        _main = this;
    }
    targetClear = true;
    enabled = true;
    distanceEvcall = 3;
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

            rSz = render->GetSize() / 2;

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

std::set<Renderer*> prev;
inline bool areaCast(Renderer* target, const Vec2Int& wpLeftTop, const Vec2Int& wpRightBottom) {
    Vec2 rSz = target->GetSize();
    Vec2 pos = target->transform()->position();
    return (pos.x + rSz.x >= wpLeftTop.x && pos.x - rSz.x <= wpRightBottom.x) &&
           (pos.y - rSz.y <= wpLeftTop.y && pos.y + rSz.y >= wpRightBottom.y);
}
std::tuple<std::set<Renderer*>*, std::set<Light*>*> Camera::matrixSelection() {
    /*       This is projection
            x-------------------
            |                   |      = * - is Vector2 (point)
            |  r * * * * * * *  |      = r - current point (ray)
            |  * * * * * * * *  |      = x - wpLeftTop
            |  * * * * * * * *  |      = y - wpRightBottom
            |  * * * * * * * *  |
            |  * * * * * * * *  |
            |  * * * * * * * *  |
            |  * * * * * * * *  |
            |                   |
             -------------------y

            Method finder: Storm
             ' * * * * * * * * *'
             ' * * * * * * * * *'   n = 10
             ' * * * * * * * * *'   n0 (first input point) = 0
             ' * * * 2 3 4 * * *'   n10 (last input point) = 9
             ' * * 9 1 0 5 * * *'
             ' * * * 8 7 6 * * *'
             ' * * * * * * * * *'
             ' * * * * * * * * *'
             ' * * * * * * * * *'
    */

    constexpr std::uint8_t Nz = 2;
    std::list<Renderer*> layers[Nz];
    std::uint8_t zN = 0;
    if (__rendererOutResults.empty()) {
        Vec2Int ray, lastRay(std::numeric_limits<int>::max(), std::numeric_limits<int>::max());
        Resolution res = Application::getResolution();
        Vec2Int wpLeftTop(Vec2::RoundToInt(this->ScreenToWorldPoint(Vec2::zero)));
        Vec2Int wpCenter(Vec2::RoundToInt(this->ScreenToWorldPoint(Vec2(res.width, res.height) / 2)));
        Vec2Int wpRightBottom(Vec2::RoundToInt(this->ScreenToWorldPoint(Vec2(res.width, res.height))));
        wpLeftTop *= distanceEvcall;
        wpRightBottom *= distanceEvcall;


        // Разработака алгоритма поиска элементов - шторм
        int step = 1;
        int steps = 1;
        char xDirection = 1;
        char yDirection = -1;
        bool rotate = false;

        //Первая точка, относительна отчета
        ray = wpCenter;
        while (true) {
            if (ray == lastRay) {
                Application::fail("overflow is lastPosition");
            }
            lastRay = ray;

            // draw current point
            auto iter = Level::self()->matrixWorld.find(ray);
            if (iter != std::end(Level::self()->matrixWorld)) {
                for (auto el : iter->second) {
                    if (Renderer* render = el->gameObject()->getComponent<Renderer>()) {
                        layers[render->zOrder].emplace_front(render);
                    }
                }
            }
            //1. Шаг заканчивается (step == steps), то происходит процедура поворота

            if (step == steps) {
                steps *= 2;
                step = 0;
                ray.y += yDirection;
                xDirection *= -1;
            } else {
                ++step;
                ray.x += xDirection;
            }
        }

        /*
        Vec2Int ray;
        Resolution res = Application::getResolution();
        Vec2Int wpLeftTop(Vec2::RoundToInt(this->ScreenToWorldPoint(Vec2::zero)));
        Vec2Int wpRightBottom(Vec2::RoundToInt(this->ScreenToWorldPoint(Vec2(res.width, res.height))));

        ray.x = wpLeftTop.x;
        while (ray.x <= wpRightBottom.x) {
            ray.y = wpLeftTop.y;
            while (ray.y >= wpRightBottom.y) {
                auto iter = Level::self()->matrixWorld.find(ray);
                if (iter != std::end(Level::self()->matrixWorld)) {
                    for (auto el : iter->second) {
                        if (Renderer* render = el->gameObject()->getComponent<Renderer>()) {
                            layers[render->zOrder].emplace_front(render);
                        }
                    }
                }
                --ray.y;
            }
            ++ray.x;
        }
        */

        std::list<Renderer*> _removes;
        //собираем оставшиеся которые прикреплены к видимости
        for (auto x = std::begin(prev); x != std::end(prev); ++x) {
            if (areaCast(*x, wpLeftTop, wpRightBottom)) {
                if (__rendererOutResults.find((*x)) == std::end(__rendererOutResults)) __rendererOutResults.insert((*x));
            } else {
                _removes.emplace_back((*x));
            }
        }

        for (Renderer* y : _removes) prev.erase(y);

        // ordering and collect
        std::list<Renderer*>* target;
        while ((target = zN < Nz ? &layers[zN++] : nullptr)) {
            for (Renderer* el : *target) {
                __rendererOutResults.insert(el);
                prev.insert(el);
            }
        }
    }

    if (__lightsOutResults.empty()) {
        __lightsOutResults.insert(RoninEngine::Level::self()->_assoc_lightings.begin(),
                                  RoninEngine::Level::self()->_assoc_lightings.end());
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
