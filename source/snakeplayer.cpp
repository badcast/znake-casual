#include "snakeplayer.h"

constexpr int tiles = 10;
Vec2 currentAxis;
Vec2* headDirection;
Vec2* headUpperBound;
Vec2 lastDirection;
float keepDistance = 0.64f;
float keepArroundDistance = 0.4f;

// TODO: optimizing
float get_quarter_angle(const Vec2& dir)
{
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
float get_arroung_angle(const Vec2& alpha, const Vec2& beta)
{
    float delta = 0;
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
        RoninEngine::Application::fail("unassigned angle for arround.");
    }
    return delta;
}

void SnakePlayer::OnAwake()
{
    playerCamera = gameObject()->add_component<Camera2D>();
    // create head
    head = create_game_object("Head");
    head->transform()->setParent(transform());
    SpriteRenderer* sel = head->add_component<SpriteRenderer>();
    sel->renderOut = SpriteRenderOut::Origin;
    sel->transform()->layer = 100;

    auto src = ResourceManager::GetSurface("snake-head");
    sel->setSpriteFromTextureToGC(src);
    // create body
    body = create_game_object("Body");
    // body->transform()->setParent(transform());
    body->transform()->localPosition(Vec2::minusOne * 0.9f);
    sel = body->add_component<SpriteRenderer>();
    src = ResourceManager::GetSurface("snake-body");
    sel->setSpriteFromTextureToGC(src);
    sel->renderOut = SpriteRenderOut::Origin;
    sel->renderType = SpriteRenderType::Simple;
    sel->renderPresentMode = SpriteRenderPresentMode::Place;
    // create arround
    arround = create_game_object("Around");
    arround->transform()->localPosition(Vec2::down * 2);
    sel = arround->add_component<SpriteRenderer>();
    src = ResourceManager::GetSurface("snake-arround");
    sel->renderOut = SpriteRenderOut::Origin;
    sel->setSpriteFromTextureToGC(src);

    znake_tiles.emplace_back(body->transform());

    for (int x = 1; x < ::tiles; x++)
        appendTile();

    create_game_object()->add_component<Camera2D>();
}

void SnakePlayer::OnStart()
{
    ZnakeBound tinfo = { 0, Vec2::up, transform()->position(), arround->transform() };
    currentAxis = lastDirection = *(headDirection = &znake_bounds.emplace_front(tinfo).direction);
    speed = 0.1;
    headUpperBound = &znake_bounds.front().upperBound;
    arround->transform()->position(Vec2::infinity);
    updatePosition();
}

void SnakePlayer::OnUpdate()
{
    // float curSpeed = input::get_key(SDL_SCANCODE_LSHIFT) ? (speed * 10) : speed;
    Vec2 newAxis = input::get_axis();
    if (newAxis != Vec2::zero && newAxis != *headDirection) {
        if (newAxis.x != 0)
            newAxis.y = 0;
        else if (newAxis.y != 0)
            newAxis.x = 0;

        if (newAxis.x - headDirection->x != 0 && newAxis.y - headDirection->y != 0)
            currentAxis = newAxis;
    }

    static float testTime = 0;

    if (TimeEngine::time() < testTime) { //|| false && Time::frame() % (input::get_key(SDL_SCANCODE_LSHIFT) ? 1 : 30) != 0) {
        return;
    }
    testTime = TimeEngine::time() + 0.1; // delay 1 seconds

    if (*headDirection != currentAxis)
        *headDirection = currentAxis;
    updatePosition();
}

void SnakePlayer::OnGizmos()
{
    int x;

    Gizmos::setColor(Color::blue);

    // Draw next line
    Gizmos::DrawLine(transform()->position(), transform()->position() + *headDirection);

    if (lastDirection != *headDirection)
        return;

    // save last position
    Vec2 last = transform()->position();

    // iteration for tiles
    for (x = 0; x < znake_tiles.size(); ++x) {
        // set position tiles
        if (x == znake_tiles.size() - 1)
            Gizmos::setColor(Color::red);
        else
            Gizmos::setColor(Color::blue);
        Gizmos::DrawPosition(znake_tiles[x]->position(), 0.3f);
        Gizmos::setColor(Color::green);
        Gizmos::DrawLine(last, znake_tiles[x]->position());
        Gizmos::DrawTextOnPosition_Legacy(znake_tiles[x]->position(), std::to_string(x));
        last = znake_tiles[x]->position();
    }

    // draw bounds
    for (auto bound = ++znake_bounds.begin(); bound != znake_bounds.end(); ++bound) {
        Gizmos::setColor(Color::red);
        Gizmos::DrawCircle(bound->upperBound, 0.25f);
        Gizmos::DrawPosition(bound->upperBound, 0.25f);
    }
}

void SnakePlayer::updatePosition()
{
    /* ***Variables*** */
    int x, y;
    auto navmesh = terrain->surfaceMesh();
    std::remove_reference_t<decltype(znake_bounds)>::iterator thisBound, backBound, nextBound;
    Vec2 lastPosition = transform()->position();
    Vec2Int npoint;
    auto nextNeuron = navmesh->GetNeuron(lastPosition + Vec2::Scale(*headDirection, navmesh->worldScale), npoint);
    /*********/

    // Удалить последний хвост который больше не требуется
    if (znake_bounds.back().boundIndex == znake_tiles.size()) {
        // Выбрать последний элемент
        znake_bounds.back().arroundGraphic->position(Vec2::infinity);
        znake_bounds.pop_back();
    }
    // Увеличить поворотные хвосты на единицу, концепция задумана как увелечение и пропуск хвостов
    // Игнорировать первую часть слежение, он же head
    for (thisBound = ++std::begin(znake_bounds); thisBound != std::end(znake_bounds);)
        ++(*thisBound++).boundIndex;

    // Данная концепция определяет новую направления для HEAD
    if (*headDirection != lastDirection) {
        Transform* newArround = instantiate(arround)->transform();
        // Создать новую часть разделения, от направление head
        //  Вставить новый хвост между HEAD и след 'хвостом'
        znake_bounds.insert(++znake_bounds.begin(), { 1, lastDirection, lastPosition, newArround });
        newArround->transform()->position(lastPosition);
        newArround->transform()->angle(get_arroung_angle(*headDirection, lastDirection));
        lastDirection = *headDirection;
    }

    // get inside <next position>
    if (!nextNeuron) {
        if (npoint.y < 0)
            npoint.y = navmesh->Height();
        else if (npoint.y > navmesh->Height())
            npoint.y = 0;

        if (npoint.x < 0)
            npoint.x = navmesh->Width();
        else if (npoint.x > navmesh->Width())
            npoint.x = 0;

        nextNeuron = navmesh->GetNeuron(npoint);
    }

    *headUpperBound = navmesh->PointToWorldPosition(nextNeuron);
    head->transform()->angle(get_quarter_angle(*headDirection)); // rotate head
    transform()->position(*headUpperBound); // move transform

    /*
        У проекций змеи, имеються схожие 2 вида хвоста
            1 - Обычные, линейные.
            2 - Поворотные на 90 градусов.
    */
    thisBound = znake_bounds.begin(); // first iterator
    backBound = --znake_bounds.end(); // last iterator
    nextBound = std::next(thisBound); // next iterator
    // Концепция направляющего
    Vec2 heuristic = thisBound->upperBound;
    //    //Главный HEAD элемент не единственный и следующий элемент, эти контексты для предоставления головы как [head->next]
    //    if (thisBound != backBound && std::next(thisBound)->first == 1) {
    //        //проигнорировать первый хвост, так как пространство для первого хваста был замещен поворотным хвостом
    //        x = 1;
    //    } else {
    //        //Ничего не произошло
    //        follow = thisBound->second.upperBound;
    //        x = 0;
    //    }

    // Выделить для каждого N видимых и N повортных 'хвостов' в 'пространстве'
    //  y - 'хвосты' которые зедействованы
    for (y = 0, x = 0; x < znake_tiles.size(); ++x) {
        // Концепция: текущий направляющий заканчивается последним хвостом, далее переходим к след. поворотному хвосту
        if (nextBound != znake_bounds.end() && nextBound->boundIndex - 1 == x) {
            // Переход к следующему поворотному хвосту
            ++thisBound;
            // Концепция дать пространство для arround (поворотной части хвоста)
            heuristic = thisBound->upperBound - thisBound->direction * keepArroundDistance;

            // Существует след. поворотный хвост после, данного?
            nextBound = std::next(thisBound);
            if (nextBound != std::end(znake_bounds)) {
                if (nextBound->boundIndex == thisBound->boundIndex + 1)
                    continue;
            } else {
            }
        } else {
            // Задаем положение хвоста в пространстве
            heuristic -= thisBound->direction * keepDistance;
        }

        // установка хвоста в положение
        znake_tiles[y]->position(heuristic);
        znake_tiles[y]->angle(get_quarter_angle(thisBound->direction));
        ++y;
    }

    // Убрать хвосты, которые не вошли в концепцию, они не вошли из за занятой ими поворотных хвостов
    for (; y < znake_tiles.size(); ++y) {
        znake_tiles[y]->transform()->position(Vec2::infinity);
    }
}

void SnakePlayer::appendTile() { znake_tiles.emplace_back(Instantiate(znake_tiles.front()->gameObject())->transform()); }
