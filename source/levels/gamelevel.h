#pragma once
#include <ronin/framework.h>
#include "../snakeplayer.h"

using namespace RoninEngine;
using namespace RoninEngine::Runtime;
using namespace RoninEngine::AI;
using namespace RoninEngine::UI;

class GameLevel : public World
{
public:
    GameLevel();
    void OnStart();
    void OnUpdate();
    void OnGizmos();
};
