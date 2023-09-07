#include "appleeat.h"
#include <fstream>

struct
{
    uid quitButton;
    uid clickButton;
    uid restore = -1;
    uid text;
} mids;
int maxWidth = 25;
NavMesh navigation(1000, 1000);

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
}

std::vector<GameObject *> apples;
MoveController2D *player;

Transform *radar_foreground;
void AppleEatGameLevel::OnStart()
{
    gui_instance = getGUI();
    const std::string defpath = "./data/sprites/";
    // create a menu
    auto appleTexture = ResourceManager::GetSurface(defpath + "apple.png");
    Rect t(0, 0, 100, 32);
    mids.quitButton = getGUI()->PushButton("Quit", t);
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
    floor->transform()->layer = -1;
    //  view->transform()->position(Vec2::infinity);

    GameObject *playerGameObject = create_game_object("Player");
    player = playerGameObject->AddComponent<MoveController2D>();
    player->AddComponent<SpriteRenderer>();
    player->spriteRenderer()->transform()->layer = 100;

    GameObject *radarObject = create_game_object("Radar");
    radarObject->transform()->set_parent(player->transform());
    SpriteRenderer *radar = radarObject->AddComponent<SpriteRenderer>();
    // radar->set_sprite(ResourceManager::make_sprite(ResourceManager::GetSurface(defpath + "radar_background.png")));
    Vec2 offset = radarObject->transform()->position();
    radarObject->transform()->position(offset);
    radarObject = Instantiate(radarObject);
    radarObject->transform()->set_parent(radar->transform(), false);
    radarObject->spriteRenderer()->set_sprite(ResourceManager::make_sprite(ResourceManager::GetSurface(defpath + "radar_foreground.png")));
    radar_foreground = radarObject->transform();
    create_game_object("Tail")->AddComponent<SpriteRenderer>();

    // Создаем N яблоко
    int n = 100000;
    int x;
    float range = maxWidth;
    GameObject *appleObject = create_game_object("apple");
    appleObject->transform()->layer = 1;
    SpriteRenderer *view2 = appleObject->AddComponent<SpriteRenderer>();
    view2->set_sprite(ResourceManager::make_sprite(appleTexture));
    view2->size = Vec2::half;
    apples.reserve(n + 1);
    apples.emplace_back(appleObject);
    for(x = 0; x < n; ++x)
    {
        appleObject = Instantiate(appleObject);
        appleObject->transform()->position(Vec2(Random::range(-range, range), Random::range(-range, range)));
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

    auto v = Camera::main_camera()->transform()->position();

    Camera::main_camera()->transform()->position(Vec2::Lerp(v, player->transform()->position(), 9 * TimeEngine::deltaTime()));

    std::vector<Transform *> finded = Physics2D::GetCircleCast<std::vector<Transform *>>(player->transform()->position(), distance, 1);
    for(int x = 0, y = Math::min<int>(128, finded.size()); x < y; ++x)
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

    for(int x = 0; ___c && x < 2; ++x)
    {
        Vec2 newPoint;
        Vec2 j = Camera::ViewportToWorldPoint(Vec2::zero);
        Vec2 k = Camera::ViewportToWorldPoint(Vec2::one);
        newPoint.x = Math::outside(Random::range((float) -maxWidth, (float) maxWidth), j.x, k.x);
        newPoint.y = Math::outside(Random::range((float) -maxWidth, (float) maxWidth), j.y, k.y);
        Instantiate(apples[0], newPoint);
    }
    return;
    apples[0]->transform()->position(Camera2D::ScreenToWorldPoint(Input::GetMousePointf()));

    if(apples.size() == 1)
        return;

    for(int i = 1; false && i < apples.size(); ++i)
    {
        apples[i]->transform()->position(Vec2(Random::range(-maxWidth, maxWidth), Random::range(-maxWidth, maxWidth)));
    }
}

void AppleEatGameLevel::OnGizmos()
{
    //    std::string tree = this->get_hierarchy_as_tree();

    //    std::ofstream ofs {"/tmp/tree_live.txt"};
    //    ofs << tree;
    //    ofs.close();

    //  Gizmos::draw_2D_world_space(Vec2::zero);

    Gizmos::SetColor(0x88241dff);

    // draw line

    static float angle = 360;

    if(angle > 360)
        angle -= 360;

    Vec2 sidePoint = player->transform()->position();

    sidePoint.x += Math::cos(angle * Math::deg2rad) * distance;
    sidePoint.y += Math::sin(angle * Math::deg2rad) * distance;
    radar_foreground->transform()->angle(angle);
    angle += 14;
    /*
        Gizmos::set_color(target_n ? Color::red : Color::green);
        Gizmos::draw_line(player->transform()->position(), sidePoint);
        Gizmos::draw_circle(player->transform()->position(), distance);
    */
    // Gizmos::draw_storm(Vec2::zero, Math::number(Math::ceil(distance)));
}
