#pragma once

#include "levels/appleeat.h"

struct ZnakeBound {
    int boundIndex;
    Vec2 direction;
    Vec2 upperBound;
    TransformRef arroundGraphic;
};

class SnakePlayer : public RoninEngine::Runtime::Behaviour {
private:
    void updatePosition();
    Vec2 currentAxis;
    Vec2* headForward;
    Vec2* headUpperBound;
    Vec2 lastDirection;
    float keepDistance = 0.64f;
    float keepArroundDistance = 0.4f;

    float moveSpeed = 0.2f;
    Vec2 targetPosition;
    Vec2 startPosition;
    float moveProgress;

public:
    float speed;
    Camera2DRef playerCamera;
    GameObjectRef head;
    GameObjectRef body;
    GameObjectRef arround;
    GameObjectRef loopbackTile;
    Terrain2DRef terrain;

    std::vector<TransformRef> znake_tiles;
    std::list<ZnakeBound> znake_bounds;

    void OnAwake() override;
    void OnStart() override;
    void OnUpdate() override;
    void OnGizmos() override;

    void appendTile();
};
