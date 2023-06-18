#pragma once
#include <ronin/framework.h>
#include "../snakeplayer.h"

using namespace RoninEngine;
using namespace RoninEngine::Runtime;
using namespace RoninEngine::AIPathFinder;
using namespace RoninEngine::UI;

class GameLevel : public World
{
public:
    GameLevel();
    void on_start();
    void on_update();
    void on_gizmo();
};
