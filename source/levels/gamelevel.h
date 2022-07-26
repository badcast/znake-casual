#pragma once
#include <ronin/framework.h>
#include "../snakeplayer.h"

using namespace RoninEngine;
using namespace RoninEngine::Runtime;
using namespace RoninEngine::AIPathFinder;
using namespace RoninEngine::UI;

class GameLevel : public Level {
   public:
    GameLevel();
    ~GameLevel() = default;
    void start() override;
    void update() override;
    void onDrawGizmos() override;
};
