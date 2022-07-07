#pragma once

#include "dependency.h"

namespace RoninEngine::Runtime {

class Physics2D {
   public:
    static std::list<Transform*> stormCast(Vec2 origin, int edges);
    static std::list<Transform*> sphereCast(Vec2 origin, float distance);

};
}  // namespace RoninEngine::Runtime
