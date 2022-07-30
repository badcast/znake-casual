#include "snakeplayer.h"

Vec2* firstDirection;
Vec2* nextPoint;
Vec2 lastMovement;

void SnakePlayer::OnAwake() {
    playerCamera = gameObject()->addComponent<Camera2D>();
    head = CreateGameObject("Head");
    head->transform()->setParent(transform());
    SpriteRenderer* sel = head->addComponent<SpriteRenderer>();
    sel->zOrder = 100;
    Texture* selTexture = GC::GetTexture("snake-head");
    sel->setSpriteFromTextureToGC(selTexture);

    body = CreateGameObject("Body");
    // body->transform()->setParent(transform());
    body->transform()->localPosition(Vec2::minusOne * 0.9f);
    sel = body->addComponent<SpriteRenderer>();
    selTexture = GC::GetTexture("snake-body");
    sel->setSpriteFromTextureToGC(selTexture);

    arround = CreateGameObject("Around");
    // arround->transform()->setParent(transform());
    arround->transform()->localPosition(Vec2::down * 2);
    sel = arround->addComponent<SpriteRenderer>();
    selTexture = GC::GetTexture("snake-arround");
    sel->setSpriteFromTextureToGC(selTexture);

    arrounds.emplace_back(arround->transform());
    tiles.emplace_back(body->transform());

    appendTile();
    appendTile();
    appendTile();
}

void SnakePlayer::OnStart() {
    speed = 0.1;
    //Всегда направляющий первый
    TileDirection tinfo = {Vec2::up, transform()->position()};
    lastMovement = *(firstDirection = &znake_bounds.emplace_front(0, tinfo).second.direction);
    nextPoint = &znake_bounds.front().second.upperBound;
    updatePosition();
}

void SnakePlayer::OnUpdate() {
    float curSpeed = input::get_key(SDL_SCANCODE_LSHIFT) ? (speed * 10) : speed;
    Vec2 axis = input::get_axis();
    if (axis != Vec2::zero) {
        if (axis.x != axis.y) {
            if (axis.x - firstDirection->x != 0 && axis.y - firstDirection->y != 0) *firstDirection = axis;

            if (firstDirection->x != 0)
                firstDirection->y = 0;
            else if (firstDirection->y != 0)
                firstDirection->x = 0;
        }
    }

    if (Time::frame() % 30 != 0) {
        return;
    }
    updatePosition();
}
float get_quarter_angle(const Vec2& dir) {
    float x = 0;

    if (dir.y < 0)
        x = 180;
    else if (dir.x > 0)
        x = 270;
    else if (dir.x < 0)
        x = 90;

    return x;
}
void SnakePlayer::OnGizmos() {
    if (lastMovement != *firstDirection) return;

    // draw tiles
    Gizmos::setColor(Color::green);

    static float keepDistance = 0.5;
    static std::vector<Vec2> tempTiles(tiles.size(), Vec2::zero);
    auto currentBound = znake_bounds.begin();
    Vec2 follow = currentBound->second.upperBound;

    for (int x = 0, count = tiles.size(); x < count; ++x) {
        Vec2 last = follow;
        // направляем хвост
        follow -= currentBound->second.direction * keepDistance;
//        tiles[x]->position(follow);
//        tiles[x]->angle(get_quarter_angle(currentBound->second.direction));
        // set position tiles
        Gizmos::setColor(Color::blue);
        Gizmos::DrawPosition(follow, 0.3f);
        Gizmos::setColor(Color::green);
        Gizmos::DrawLine(last, follow);

        // Draw rotate state
        if (znake_bounds.size() > 1 && currentBound != --std::end(znake_bounds)) {
            if (currentBound->first == x) {
                //select next bound
                ++currentBound;

                if (currentBound->first == 3) {
                    int ccc = 0;
                }
                //assign bound position
                follow = currentBound->second.upperBound;
                Gizmos::setColor(Color::red);
                Gizmos::DrawCircle(follow, 0.25f);
                Gizmos::DrawPosition(follow, 0.25f);
                Gizmos::setColor(Color::green);
            }
        }
    }
}



void SnakePlayer::updatePosition() {
    auto navmesh = terrain->getNavMesh();
    Vec2 lastPosition = transform()->position();
    Vec2Int npoint;
    AIPathFinder::Neuron* nextNeuron =
        navmesh->GetNeuron(lastPosition + Vec2::Scale(*firstDirection, navmesh->worldScale), npoint);

    //Удалить последний хвост который больше не требуется
    if (znake_bounds.back().first == tiles.size()) znake_bounds.pop_back();

    //Увеличить все части поворотов на 1, когда хвосты двигаются вперед
    // Игнорировать 1 часть слежение, он же head
    for (auto x = ++std::begin(znake_bounds); x != std::end(znake_bounds); ++(*x++).first)
        ;

    //Произошло вращение
    if (*firstDirection != lastMovement) {
        //Создать новую часть разделения, от изменения направление head
        TileDirection newBound = {lastMovement, lastPosition};
        znake_bounds.insert(++znake_bounds.begin(), std::make_pair(1, newBound));
        lastMovement = *firstDirection;
    }

    if (!nextNeuron) {
        if (npoint.y < 0)
            npoint.y = navmesh->heightSpace;
        else if (npoint.y > navmesh->heightSpace)
            npoint.y = 0;

        if (npoint.x < 0)
            npoint.x = navmesh->widthSpace;
        else if (npoint.x > navmesh->widthSpace)
            npoint.x = 0;

        nextNeuron = navmesh->GetNeuron(npoint);
    }

    *nextPoint = navmesh->PointToWorldPosition(nextNeuron);
    head->transform()->angle(get_quarter_angle(*firstDirection));  // rotate head
    transform()->position(*nextPoint);                             // move transform
}

void SnakePlayer::appendTile() { tiles.emplace_back(Instantiate(tiles.front()->gameObject())->transform()); }
