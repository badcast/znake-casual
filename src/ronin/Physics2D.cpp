#include "Physics2D.h"

#include "framework.h"

namespace RoninEngine::Runtime {

std::list<Transform*> Physics2D::sphereCast(Vec2 origin, float distance) {
    auto& mx = Level::self()->matrixWorld;
    std::list<Transform*> _cont;

    Vec2Int env;
    Vec2Int originRounded = Vec2::RoundToInt(origin);
    float distanceRounded = Mathf::ceil(distance);
    for (env.x = originRounded.x - distanceRounded; env.x <= originRounded.x + distanceRounded; ++env.x) {
        for (env.y = originRounded.y - distanceRounded; env.y <= originRounded.y + distanceRounded; ++env.y) {
            auto findedIter = mx.find(env);
            if (findedIter != std::end(mx)) {
                // filtering by distance
                for (auto lhs : findedIter->second) {
                    if (Vec2::Distance(lhs->_p, origin) <= distance)
                        _cont.emplace_back(lhs);
                }
            }
        }
    }

    return _cont;
}

}  // namespace RoninEngine::Runtime
