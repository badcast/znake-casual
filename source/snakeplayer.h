#ifndef SNAKEPLAYER_H
#define SNAKEPLAYER_H

#include <ronin/framework.h>

struct TileDirection{
    Vec2 direction;
    Vec2 upperBound;
};

class SnakePlayer : public RoninEngine::Runtime::Behaviour
{
public:
    float speed;
    Camera2D *playerCamera;
    GameObject* head;
    GameObject* body;
    GameObject* arround;
    GameObject* loopbackTile;
    Terrain2D * terrain;
<<<<<<< HEAD
    std::list<std::pair<int, TileDirection>> znake_bounds;
=======
    std::list<std::pair<int, TileDirection>> bounds;
>>>>>>> c349edcdaa2e1e234fcc9bad4e105a8885122f05
    std::vector<Transform*> tiles;
    std::list<Transform*> arrounds;

    void OnAwake() override;
    void OnStart() override;
    void OnUpdate() override;
    void OnGizmos() override;

    void updatePosition();

    void appendTile();
};

#endif // SNAKEPLAYER_H
