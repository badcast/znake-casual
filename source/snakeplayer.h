#pragma once

#include <ronin/framework.h>

struct ZnakeBound {
    int boundIndex;
    Vec2 direction;
    Vec2 upperBound;
    Transform* arroundGraphic;
};

class SnakePlayer : public RoninEngine::Runtime::Behaviour {
   private:
    void updatePosition();

   public:
    float speed;
    Camera2D* playerCamera;
    GameObject* head;
    GameObject* body;
    GameObject* arround;
    GameObject* loopbackTile;
    Terrain2D* terrain;

    std::vector<Transform*> znake_tiles;
    std::list<ZnakeBound> znake_bounds;

    void OnAwake() override;
    void OnStart() override;
    void OnUpdate() override;
    void OnGizmos() override;

    void appendTile();
};
