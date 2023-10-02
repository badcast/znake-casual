#include "snakeplayer.h"

constexpr int tiles = 10;


float headAngleRotate(const Vec2& dir)
{
    return (dir.y < 0) ? 180.0f :
               (dir.x > 0) ? 90.0f  :
               (dir.x < 0) ? -90.0f :
               0.0f;
}

float boundAngleRotate(const Vec2& alpha, const Vec2& beta)
{
    float delta = 0;
    // turn on right
    if (alpha.x > 0) {
        if (beta.y > 0)
            delta = 90;
        else if (beta.y < 0)
            delta = 0;

    }
    // turn on left
    else if (alpha.x < 0) {
        if (beta.y > 0)
            delta = 180;
        else if (beta.y < 0)
            delta = 270;
    }
    // turn on up
    else if (alpha.y > 0) {
        if (beta.x > 0)
            delta = 270;
        else if (beta.x < 0)
            delta = 0;
    }
    // turn on down
    else if (alpha.y < 0) {
        if (beta.x > 0)
            delta = 180;
        else if (beta.x < 0)
            delta = 90;
    }
    return delta;
}

void SnakePlayer::OnAwake()
{
    playerCamera = gameObject()->AddComponent<Camera2D>();
    // create head
    head = Primitive::CreateEmptyGameObject("Head");
    head->transform()->setParent(transform());
    SpriteRendererRef sel = head->AddComponent<SpriteRenderer>();
    sel->setRenderOut(SpriteRenderOut::Origin);
    sel->transform()->layer (100);

    std::string default_path = "./data/sprites/";

    auto src = ResourceManager::GetSurface(default_path + "snake-head.png");
    sel->setSprite(ResourceManager::make_sprite(src));

    // create body
    body = Primitive::CreateEmptyGameObject("Body");

    sel = body->AddComponent<SpriteRenderer>();
    src = ResourceManager::GetSurface(default_path + "snake-body.png");
    sel->setSprite(ResourceManager::make_sprite(src));

    sel->setRenderOut(SpriteRenderOut::Origin);
    sel->setRenderType(SpriteRenderType::Simple);
    sel->setPresentMode(SpriteRenderPresentMode::Place);

    // create arround
    arround = Primitive::CreateEmptyGameObject("Around");
    arround->transform()->localPosition(Vec2::down * 2);
    sel = arround->AddComponent<SpriteRenderer>();
    src = ResourceManager::GetSurface(default_path + "snake-arround.png");
    sel->setRenderOut(SpriteRenderOut::Origin);
    sel->setSprite(ResourceManager::make_sprite(src));

    znake_tiles.emplace_back(body->transform());

    for (int x = 1; x < ::tiles; x++)
        appendTile();
    targetPosition = transform()->position();
    startPosition = targetPosition;
    moveProgress = 1;
}

void SnakePlayer::OnStart()
{
    ZnakeBound tinfo = { 0, Vec2::up, transform()->position(), arround->transform() };
    currentAxis = lastDirection = *(headForward = &znake_bounds.emplace_front(std::move(tinfo)).direction);
    headUpperBound = &znake_bounds.front().upperBound;
    arround->transform()->position(Vec2::infinity);
    updatePosition();
}

void SnakePlayer::OnUpdate()
{
    Vec2 newAxis = Input::GetAxis();
    if (newAxis != Vec2::zero && newAxis != *headForward) {
        if (newAxis.x != 0)
            newAxis.y = 0;
        else if (newAxis.y != 0)
            newAxis.x = 0;

        if (newAxis.x - headForward->x != 0 && newAxis.y - headForward->y != 0)
            currentAxis = newAxis;
    }

    if (moveProgress < 1.0f)
    {
        moveProgress += moveSpeed * Time::deltaTime() * 10.0f;

        if (moveProgress >= 1.0f) {
            moveProgress = 1.0f;
        }

        // Интерполируем (плавно перемещаем) позицию головы между стартовой и целевой
        transform()->position(Vec2::Lerp(startPosition, targetPosition, moveProgress));
        znake_tiles.back()->spriteRenderer()->setSize(Vec2::Lerp(znake_tiles.back()->spriteRenderer()->getSize(), Vec2::zero, 1-moveProgress));
    }
    else
    {
        if (*headForward != currentAxis)
            *headForward = currentAxis;
        updatePosition();
        moveProgress = 0.0f;
    }
}


void SnakePlayer::OnGizmos()
{
    if (lastDirection != *headForward)
        return;

    // save last position
    Vec2 last = transform()->position();

    // iteration for tiles
    RenderUtility::SetColor(Color::blue);
    for (int x = 0; x < znake_tiles.size(); ++x) {
        // set position tiles
        if (x == znake_tiles.size() - 1)
            RenderUtility::SetColor(Color::red);
        else
            RenderUtility::SetColor(Color::blue);
        RenderUtility::DrawPosition(znake_tiles[x]->position(), 0.3f);
        RenderUtility::SetColor(Color::green);
        RenderUtility::DrawLine(last, znake_tiles[x]->position());
        RenderUtility::DrawText(znake_tiles[x]->position(), std::to_string(x));
        last = znake_tiles[x]->position();
    }

    // draw bounds
    for (auto bound = ++znake_bounds.begin(); bound != znake_bounds.end(); ++bound) {
        RenderUtility::SetColor(Color::red);
        RenderUtility::DrawCircle(bound->upperBound, 0.25f);
        RenderUtility::DrawPosition(bound->upperBound, 0.25f);
    }
}

void SnakePlayer::updatePosition()
{
    int x, y;
    auto navmesh = terrain->get_navmesh2D();
    std::remove_reference_t<decltype(znake_bounds)>::iterator thisBound, backBound, nextBound;
    Vec2 lastPosition = transform()->position();
    Vec2Int npoint;
    auto nextNeuron = navmesh->get(lastPosition + Vec2::Scale(*headForward, navmesh->worldScale), npoint);

    // Удалить последний хвост который больше не требуется
    if (znake_bounds.back().boundIndex == znake_tiles.size()) {
        // Выбрать последний элемент
        znake_bounds.back().arroundGraphic->gameObject()->Destroy();
        znake_bounds.pop_back();
    }
    // Увеличить поворотные хвосты на единицу, концепция задумана как увелечение и пропуск хвостов
    // Игнорировать первую часть слежение, он же head
    for (thisBound = ++std::begin(znake_bounds); thisBound != std::end(znake_bounds);)
        ++(*thisBound++).boundIndex;

    // Данная концепция определяет новую направления для HEAD
    if (*headForward != lastDirection) {
        TransformRef newArround = Instantiate(arround)->transform();
        // Создать новую часть разделения, от направление head
        //  Вставить новый хвост между HEAD и след 'хвостом'
        znake_bounds.insert(++znake_bounds.begin(), { 1, lastDirection, lastPosition, newArround });
        newArround->transform()->position(lastPosition);
        newArround->transform()->angle(boundAngleRotate(*headForward, lastDirection));
        newArround->transform()->layer(transform()->layer()+1);
        lastDirection = *headForward;
    }

    // get inside <next position>
    if (!nextNeuron) {
        if (npoint.y < 0)
            npoint.y = navmesh->getHeight();
        else if (npoint.y > navmesh->getHeight())
            npoint.y = 0;

        if (npoint.x < 0)
            npoint.x = navmesh->getWidth();
        else if (npoint.x > navmesh->getWidth())
            npoint.x = 0;

        nextNeuron = navmesh->get(npoint);
    }


    Vec2 newTilePosition;
    startPosition = lastPosition;
    newTilePosition = lastPosition;
    *headUpperBound = navmesh->NeuronToWorldPoint(nextNeuron);
    targetPosition = *headUpperBound;
    head->transform()->angle(headAngleRotate(*headForward)); // rotate head
    znake_tiles.back()->spriteRenderer()->setSize(Vec2::one * keepDistance);

    /*
        У проекций змеи, имеються схожие 2 вида хвоста
            1 - Обычные, линейные.
            2 - Поворотные на 90 градусов.
    */
    thisBound = znake_bounds.begin(); // first iterator
    backBound = --znake_bounds.end(); // last iterator
    nextBound = std::next(thisBound); // next iterator

    // Концепция направляющего
    newTilePosition = thisBound->upperBound;

    // Выделить для каждого N видимых и N повортных 'хвостов' в 'пространстве'
    //  y - 'хвосты' которые зедействованы
    for (x = 0; x < znake_tiles.size(); ++x) {
        // установка хвоста в положение
        znake_tiles[x]->position(newTilePosition);
        znake_tiles[x]->angle(headAngleRotate(thisBound->direction));

        // Концепция: текущий направляющий заканчивается последним хвостом, далее переходим к след. поворотному хвосту
        if (nextBound != znake_bounds.end() && nextBound->boundIndex - 1 == x) {
            // Переход к следующему поворотному хвосту
            ++thisBound;
            // Концепция дать пространство для arround (поворотной части хвоста)
            newTilePosition = thisBound->upperBound - thisBound->direction * keepArroundDistance;

            // Существует след. поворотный хвост после, данного?
            nextBound = std::next(thisBound);
            if (nextBound != std::end(znake_bounds) && nextBound->boundIndex == thisBound->boundIndex + 1) {
                continue;
            }
        } else {
            // Задаем положение хвоста в пространстве
            newTilePosition -= thisBound->direction * keepDistance;
        }
    }
}

void SnakePlayer::appendTile() { znake_tiles.emplace_back(Instantiate(znake_tiles.front()->gameObject())->transform()); }
