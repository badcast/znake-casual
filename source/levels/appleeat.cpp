#include "appleeat.h"
#include <fstream>

struct
{
    uid quitButton;
    uid reloadButton;
    uid clickButton;
    uid restore = -1;
    uid text;
} mids;

int maxWidth = 7;
int createWith = 3;
int createStart = 1000;

AppleEatGameLevel::AppleEatGameLevel() : World("Apple Eater")
{
}
GUI *gui_instance;
void callback(uid id, void *userData)
{
    if(id == mids.quitButton)
        RoninSimulator::RequestQuit();
    else if(id == mids.clickButton)
    {
        Rect rect = gui_instance->GetElementRect(id);
        rect.w = 200;
        gui_instance->SetElementRect(id, rect);
        auto damaged = World::self()->matrix_check_damaged();
        gui_instance->SetElementText(id, "Damaged: " + std::to_string(damaged.size()));

        if(mids.restore == -1)
        {
            rect.x += rect.w;
            rect.w = 100;
            mids.restore = gui_instance->PushButton("Restore", rect);
        }

        gui_instance->SetElementVisible(mids.restore, true);
    }
    else if(id == mids.restore)
    {
        gui_instance->SetElementVisible(mids.restore, false);
        World::self()->matrix_restore();
    }
    else if(id == mids.reloadButton)
    {
        RoninSimulator::ReloadWorld();
    }
}

std::vector<GameObject *> apples;
MoveController2D *player;

Transform *radar_foreground;
void AppleEatGameLevel::OnStart()
{
    apples = {};
    gui_instance = getGUI();
    const std::string defpath = "./data/sprites/";
    // create a menu
    auto appleTexture = ResourceManager::GetSurface(defpath + "apple.png");
    Rect t(0, 0, 100, 32);
    mids.quitButton = getGUI()->PushButton("Quit", t);
    float y = t.y;
    t.y += t.y + t.h;
    mids.reloadButton = getGUI()->PushButton("Reload world", t);
    t.y = y;
    t.x += t.x + t.w;
    mids.clickButton = getGUI()->PushButton("Check", t);
    t.x += t.x + t.w;

    t.w = t.h = 32;

    // guiInstance->Push_TextureStick(appleTexture, t);

    getGUI()->SetGeneralCallback(callback, nullptr);

    Resolution res = RoninSimulator::GetCurrentResolution();
    t.w = 200;
    t.x = res.width - t.w;
    mids.text = getGUI()->PushButton("", t);
    t.y += 32;
    getGUI()->PushButton(
        "Clear matrix cache",
        t,
        [](uid id)
        {
            RoninSimulator::ShowMessage(
                "Кэш очищен, ваша производительность может быть понижена.\nКэш: " + std::to_string(World::self()->matrix_clear_cache()) +
                " блоков");
        });
    // instance games
    auto floorTexture = ResourceManager::GetSurface(defpath + "concrete.jpg");

    GameObject *floor = create_game_object("Floor");
    SpriteRenderer *view = floor->AddComponent<SpriteRenderer>();
    view->set_sprite(ResourceManager::make_sprite(floorTexture));
    view->size = Vec2::one * 7;
    // view->renderType = SpriteRenderType::Tile;
    view->renderPresentMode = SpriteRenderPresentMode::Place;
    floor->transform()->layer(-10);
    //  view->transform()->position(Vec2::infinity);

    GameObject *playerGameObject = create_game_object("Player");
    player = playerGameObject->AddComponent<MoveController2D>();
    player->AddComponent<SpriteRenderer>();
    player->spriteRenderer()->transform()->layer(100);

    GameObject *radarObject = create_game_object("Radar");
    radarObject->transform()->setParent(player->transform());
    SpriteRenderer *radar = radarObject->AddComponent<SpriteRenderer>();
    // radar->set_sprite(ResourceManager::make_sprite(ResourceManager::GetSurface(defpath + "radar_background.png")));
    Vec2 offset = radarObject->transform()->position();
    radarObject->transform()->position(offset);
    radarObject = Instantiate(radarObject);
    radarObject->transform()->setParent(radar->transform(), false);
    radarObject->spriteRenderer()->set_sprite(ResourceManager::make_sprite(ResourceManager::GetSurface(defpath + "radar_foreground.png")));
    radar_foreground = radarObject->transform();
    create_game_object("Tail")->AddComponent<SpriteRenderer>();

    // Создаем N яблоко
    int n = createStart;
    int x;
    float range = maxWidth;
    GameObject *appleObject = create_game_object("apple");
    appleObject->transform()->layer(1);
    SpriteRenderer *view2 = appleObject->AddComponent<SpriteRenderer>();
    view2->set_sprite(ResourceManager::make_sprite(appleTexture));
    view2->size = Vec2::half;
    apples.reserve(n + 1);
    apples.emplace_back(appleObject);
    for(x = 0; x < n; ++x)
    {
        appleObject = Instantiate(appleObject);
        appleObject->transform()->position(Vec2(Random::Range(-range, range), Random::Range(-range, range)));
        apples.emplace_back(appleObject);
    }
}

int score = 0;
int target_n = 0;
float distance = 3;
void AppleEatGameLevel::OnUpdate()
{
    int culled = this->GetCulled();
    getGUI()->SetElementText(mids.text, "Render: " + std::to_string(culled));

    std::string temp;
    temp = "Score: ";
    temp += std::to_string(score);
    getGUI()->SetElementText(mids.text, temp);

    Vec2 cameraPosition = Camera::mainCamera()->transform()->position();

    Camera::mainCamera()->transform()->position(Vec2::Lerp(cameraPosition, player->transform()->position(), 9 * TimeEngine::deltaTime()));

    std::vector<Transform *> finded = Physics2D::GetCircleCast<std::vector<Transform *>>(player->transform()->position(), distance, 1);
    for(int x = 0, y = Math::Min<int>(128, finded.size()); x < y; ++x)
    {
        Transform *t;
        Transform *f = finded.at(x);
        if(f->gameObject()->name() != "apple (clone)")
            continue;

        f->transform()->position(
            Vec2::MoveTowards(f->transform()->position(), player->transform()->position(), 15 * TimeEngine::deltaTime()));
        if(Vec2::DistanceSqr(player->transform()->position(), f->position()) < 0.3f * 0.3f)
            t = f;
        else
            t = nullptr;
        // Draw point destroyer
        // Gizmos::DrawLine(f->position(), player->transform()->position());

        if(t)
        {
            /*
                Vec2 newPoint;
                Vec2 j = Camera::viewport_to_world(Vec2::zero);
                Vec2 k = Camera::viewport_to_world(Vec2::one);
                newPoint.x = Math::outside(Random::range((float)-maxWidth, (float)maxWidth), j.x, k.x);
                newPoint.y = Math::outside(Random::range((float)-maxWidth, (float)maxWidth), j.y, k.y);
                t->position(newPoint);*/
            t->gameObject()->Destroy();
            ++score;
        }
        else
        {
            ++target_n;
        }
    }

    static bool ___c = true;

    if(Input::GetMouseUp(MouseState::MouseMiddle))
        ___c = !___c;

    for(int x = 0; ___c && x < createWith; ++x)
    {
        Vec2 newPoint;
        Vec2 j = Camera::ViewportToWorldPoint(Vec2::zero);
        Vec2 k = Camera::ViewportToWorldPoint(Vec2::one);
        newPoint.x = Math::Outside(Random::Range((float) -maxWidth, (float) maxWidth), j.x, k.x);
        newPoint.y = Math::Outside(Random::Range((float) -maxWidth, (float) maxWidth), j.y, k.y);
        Instantiate(apples[0], newPoint);
    }
}

void AppleEatGameLevel::OnGizmos()
{
    Gizmos::SetColor(0x88241dff);

    // draw line

    static float angle = 360;

    if(angle > 360)
        angle -= 360;

    Vec2 sidePoint = player->transform()->position();

    sidePoint.x += Math::Cos(angle * Math::deg2rad) * distance;
    sidePoint.y += Math::Sin(angle * Math::deg2rad) * distance;
    radar_foreground->transform()->angle(angle);
    angle += 9;

    Gizmos::DrawTextLegacy(Camera::ViewportToWorldPoint(Vec2::up / 2), "Apples: " + std::to_string(this->GetCulled()));
}
