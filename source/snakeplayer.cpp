#include "snakeplayer.h"

constexpr int tiles = 10;
Vec2* firstDirection;
Vec2* nextPoint;
Vec2 lastMovement;
float keepDistance = 0.64f;
float keepArroundDistance = 0.4f;

Transform* pushNewArroud(SnakePlayer* player) {
    Transform* newArround = Instantiate(player->arround)->transform();
    player->arrounds.emplace_back(newArround);
    return newArround;
}

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

    for (int x = 0; x < ::tiles; x++) appendTile();
}

void SnakePlayer::OnStart() {
    TileDirection tinfo = {Vec2::up, transform()->position()};
    lastMovement = *(firstDirection = &znake_bounds.emplace_front(0, tinfo).second.direction);
    speed = 0.1;
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

    if (Time::frame() % (input::get_key(SDL_SCANCODE_LSHIFT) ? 5 : 30) != 0) {
        return;
    }
    updatePosition();
}
// TODO: optimizing
float get_quarter_angle(const Vec2& dir) {
    float alpha;
    if (dir.y < 0)
        alpha = 180;
    else if (dir.x > 0)
        alpha = 270;
    else if (dir.x < 0)
        alpha = 90;
    else
        alpha = 0;
    return alpha;
}

// TODO: optimizing
float get_arroung_angle(const Vec2& alpha, const Vec2& beta) {
    float delta;
    // turn on right
    if (alpha.x > 0) {
        if (beta.y > 0)
            delta = 270;
        else if (beta.y < 0)
            delta = 0;

    }
    // turn on left
    else if (alpha.x < 0) {
        if (beta.y > 0)
            delta = 180;
        else if (beta.y < 0)
            delta = 90;

    }
    // turn on up
    else if (alpha.y > 0) {
        if (beta.x > 0)
            delta = 90;
        else if (beta.x < 0)
            delta = 0;
    }
    // turn on down
    else if (alpha.y < 0) {
        if (beta.x > 0)
            delta = 180;
        else if (beta.x < 0)
            delta = 270;
    } else {
        Application::fail("unassigned angle for arround.");
        delta = 0;
    }
    return delta;
}
void SnakePlayer::OnGizmos() {
    Gizmos::setColor(Color::blue);

    Gizmos::DrawArrow(transform()->position(), *firstDirection);

    if (lastMovement != *firstDirection) return;

    // save last position
    Vec2 last = transform()->position();

    // iteration for tiles
    for (int x = 0; x < tiles.size(); ++x) {
        // set position tiles
        if (x == tiles.size() - 1)
            Gizmos::setColor(Color::red);
        else
            Gizmos::setColor(Color::blue);
        Gizmos::DrawPosition(tiles[x]->position(), 0.3f);
        Gizmos::setColor(Color::green);
        Gizmos::DrawLine(last, tiles[x]->position());
        last = tiles[x]->position();
    }

    // draw bounds
    for (auto bound = ++znake_bounds.begin(); bound != znake_bounds.end(); ++bound) {
        Gizmos::setColor(Color::red);
        Gizmos::DrawCircle(bound->second.upperBound, 0.25f);
        Gizmos::DrawPosition(bound->second.upperBound, 0.25f);
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
        Transform* newArround = pushNewArroud(this);
        newArround->transform()->position(lastPosition);
        newArround->transform()->angle(get_arroung_angle(*firstDirection, lastMovement));
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

    auto currentBound = znake_bounds.begin();
    auto backBound = --znake_bounds.end();
    Vec2 follow = currentBound->second.upperBound;
    int x, y = 0;
    if (currentBound != backBound && std::next(currentBound)->first == 1) {
        follow -= currentBound->second.direction * keepArroundDistance;
        x = 1;
    } else
        x = 0;
    // iteration for tiles
    for (; x < tiles.size(); ++x) {
        // Draw rotate state
        if (currentBound != backBound && std::next(currentBound)->first == x) {
            // select next bound
            ++currentBound;
            // assign bound position
            follow = currentBound->second.upperBound - keepArroundDistance * currentBound->second.direction;
        }
        // направляем хвост за связыванием (bound)
        else {
            follow -= currentBound->second.direction * keepDistance;
        }
        tiles[y]->position(follow);
        tiles[y]->angle(get_quarter_angle(currentBound->second.direction));
        ++y;
    }
    // disallow isn't tiles
    for (; y < tiles.size(); ++y) {
        tiles[y]->transform()->position(Vec2::infinity);
    }
}

void SnakePlayer::appendTile() { tiles.emplace_back(Instantiate(tiles.front()->gameObject())->transform()); }
