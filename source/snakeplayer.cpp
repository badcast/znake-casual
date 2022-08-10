#include "snakeplayer.h"

constexpr int tiles = 2;
Vec2 currentAxis;
Vec2* firstDirection;
Vec2* firstUpperBound;
Vec2 lastDirection;
float keepDistance = 0.64f;
float keepArroundDistance = 0.4f;

void SnakePlayer::OnAwake() {
    // playerCamera = gameObject()->addComponent<Camera2D>();
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

    for (int x = 1; x < ::tiles; x++) appendTile();

    CreateGameObject()->addComponent<Camera2D>();
}

void SnakePlayer::OnStart() {
    TileDirection tinfo = {Vec2::up, transform()->position()};
    currentAxis = lastDirection = *(firstDirection = &znake_bounds.emplace_front(0, tinfo).second.direction);
    speed = 0.1;
    firstUpperBound = &znake_bounds.front().second.upperBound;
    arround->transform()->position(Vec2::infinity);
    updatePosition();
}

void SnakePlayer::OnUpdate() {
    float curSpeed = input::get_key(SDL_SCANCODE_LSHIFT) ? (speed * 10) : speed;
    Vec2 newAxis = input::get_axis();
    if (newAxis != Vec2::zero && newAxis != *firstDirection) {
        if (newAxis.x != 0)
            newAxis.y = 0;
        else if (newAxis.y != 0)
            newAxis.x = 0;

        if (newAxis.x - firstDirection->x != 0 && newAxis.y - firstDirection->y != 0) currentAxis = newAxis;
    }

    static float testTime = 0;

    if (Time::time() < testTime || false && Time::frame() % (input::get_key(SDL_SCANCODE_LSHIFT) ? 1 : 30) != 0) {
        return;
    }
    testTime = Time::time() + 1;  // delay 1 seconds

    if (*firstDirection != currentAxis) *firstDirection = currentAxis;
    updatePosition();
}

Transform* pushNewArroud(SnakePlayer* player) {
    Transform* newArround;
    auto iter = player->arrounds.begin();
    std::advance(iter, player->znake_bounds.size() - 2);
    if (iter == std::end(player->arrounds)) {
        newArround = Instantiate(player->arround)->transform();
        player->arrounds.emplace_front(newArround);
    } else {
        newArround = *iter;
    }

    return newArround;
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
    return;
    Gizmos::setColor(Color::blue);

    // Draw next line
    Gizmos::DrawLine(transform()->position(), transform()->position() + *firstDirection);

    if (lastDirection != *firstDirection) return;

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
    int x, y;
    auto navmesh = terrain->getNavMesh();
    std::list<std::pair<int, TileDirection>>::iterator thisBound, backBound;
    Vec2 lastPosition = transform()->position();
    Vec2Int npoint;
    auto nextNeuron = navmesh->GetNeuron(lastPosition + Vec2::Scale(*firstDirection, navmesh->worldScale), npoint);

    //Удалить последний хвост который больше не требуется
    if (znake_bounds.back().first == tiles.size()) {
        auto iter = arrounds.begin();
        std::advance(iter, znake_bounds.size() - 2);
        (*iter)->position(Vec2::infinity);
        znake_bounds.pop_back();
    }
    //Увеличить поворотные хвосты на единицу, концепция задумана как увелечение и пропуск хвостов
    //Игнорировать первую часть слежение, он же head
    for (thisBound = ++std::begin(znake_bounds); thisBound != std::end(znake_bounds);) ++(*thisBound++).first;

    //Данная концепция определяет новую направления для HEAD
    if (*firstDirection != lastDirection) {
        //Создать новую часть разделения, от направление head
        TileDirection newBound = {lastDirection, lastPosition};
        znake_bounds.insert(++znake_bounds.begin(), std::make_pair(1, newBound));
        Transform* newArround = pushNewArroud(this);
        newArround->transform()->position(lastPosition);
        newArround->transform()->angle(get_arroung_angle(*firstDirection, lastDirection));
        lastDirection = *firstDirection;
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

    *firstUpperBound = navmesh->PointToWorldPosition(nextNeuron);
    head->transform()->angle(get_quarter_angle(*firstDirection));  // rotate head
    transform()->position(*firstUpperBound);                       // move transform

    /*
        У змейки есть 2 вида хвоста
            1 - Обычные, линейные.
            2 - Поворотные на 90 градусов.
    */
    thisBound = znake_bounds.begin();
    backBound = --znake_bounds.end();

    //Конепция направляющего
    Vec2 follow = thisBound->second.upperBound;
    //    //Главный HEAD элемент не единственный и следующий элемент, эти контексты для предоставления головы как [head->next]
    //    if (thisBound != backBound && std::next(thisBound)->first == 1) {
    //        //проигнорировать первый хвост, так как пространство для первого хваста был замещен поворотным хвостом
    //        x = 1;
    //    } else {
    //        //Ничего не произошло
    //        follow = thisBound->second.upperBound;
    //        x = 0;
    //    }

    //Выделить для каждого N видимых и N повортных хвостов пространства в 2D мире
    // y - фактическое используемые хвосты
    for (y = 0, x = 0; x < tiles.size(); ++x) {
        auto nextBound = std::next(thisBound);
        //Концепция: текущий направляющий заканчивается последним хвостом, далее переходим к след. поворотному хвосту
        if (nextBound != znake_bounds.end() && nextBound->first - 1 == x) {
            //Переход к следующему поворотному хвосту
            ++thisBound;
            //Концепция дать пространство для arround (поворотной части хвоста)
            follow = thisBound->second.upperBound - thisBound->second.direction * keepArroundDistance;
        } else {
            //Задаем положение хвоста в пространстве
            follow -= thisBound->second.direction * keepDistance;
        }
        tiles[y]->position(follow);
        tiles[y]->angle(get_quarter_angle(thisBound->second.direction));
        ++y;
    }

    //Убрать хвосты, которые не вошли в концепцию, они не вошли из за занятой ими поворотных хвостов
    for (; y < tiles.size(); ++y) {
        tiles[y]->transform()->position(Vec2::infinity);
    }
}

void SnakePlayer::appendTile() { tiles.emplace_back(Instantiate(tiles.front()->gameObject())->transform()); }
