#pragma once

#include <ronin/framework.h>

class Terrain2DEditor : public RoninEngine::Level
{
public:
    void start() override;
    void update() override;
    void onDrawGizmos() override;
};
