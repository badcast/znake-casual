#include "appleeat.h"
#include <fstream>

struct {
    uid quitButton;
    uid clickButton;
    uid restore = -1;
    uid text;
} mids;
int maxWidth = 25;
NavMesh navigation(1000, 1000);

AppleEatGameLevel::AppleEatGameLevel()
    : World("Apple Eater")
{
}
GUI* gui_instance;
void callback(uid id, void* userData)
{
    if (id == mids.quitButton)
        RoninSimulator::request_quit();
    else if (id == mids.clickButton) {
        Rect r = gui_instance->get_rect(id);
        r.w = 200;
        gui_instance->set_rect(id, r);
        auto damaged = World::self()->matrix_check_damaged();
        gui_instance->set_text(id, "Damaged: " + std::to_string(damaged.size()));

        if (mids.restore == -1) {
            r.x += r.w;
            r.w = 100;
            mids.restore = gui_instance->push_button("Restore", r);
        }

        gui_instance->set_visible(mids.restore, true);

    } else if (id == mids.restore) {
        gui_instance->set_visible(mids.restore, false);
        World::self()->matrix_restore();
    }
}

std::vector<GameObject*> apples;
MoveController2D* player;

Transform* radar_foreground;
void AppleEatGameLevel::on_start()
{
    gui_instance = get_gui();
    const std::string defpath = "./data/sprites/";
    // create a menu
    auto appleTexture = ResourceManager::GetSurface(defpath + "apple.png");
    if (true) {
        Rect t(0, 0, 100, 32);
        mids.quitButton = get_gui()->push_button("Quit", t);
        t.x += t.x + t.w;
        mids.clickButton = get_gui()->push_button("Check", t);
        t.x += t.x + t.w;

        t.w = t.h = 32;

        // guiInstance->Push_TextureStick(appleTexture, t);

        get_gui()->register_general_callback(callback, nullptr);

        Resolution res = RoninSimulator::get_current_resolution();
        t.w = 200;
        t.x = res.width - t.w;
        mids.text = get_gui()->push_button("", t);
        t.y += 32;
        get_gui()->push_button("Clear matrix cache", t, [](uid id) { RoninSimulator::show_message("Кэш очищен, ваша производительность может быть понижена.\nКэш: " + std::to_string(World::self()->matrix_clear_cache()) + " блоков"); });
    }
    // instance games
    auto floorTexture = ResourceManager::GetSurface(defpath + "concrete.jpg");

    GameObject* floor = create_game_object("Floor");
    SpriteRenderer* view = floor->add_component<SpriteRenderer>();
    view->set_sprite(ResourceManager::make_sprite(floorTexture));
    view->size = Vec2::one * 7;
    // view->renderType = SpriteRenderType::Tile;
    view->renderPresentMode = SpriteRenderPresentMode::Place;
    floor->transform()->layer = -1;
    //  view->transform()->position(Vec2::infinity);

    GameObject* playerGameObject = create_game_object("Player");
    player = playerGameObject->add_component<MoveController2D>();
    player->add_component<SpriteRenderer>();
    player->get_sprite_renderer()->transform()->layer = 100;

    GameObject* radarObject = create_game_object("Radar");
    radarObject->transform()->set_parent(player->transform());
    SpriteRenderer* radar = radarObject->add_component<SpriteRenderer>();
    // radar->set_sprite(ResourceManager::make_sprite(ResourceManager::GetSurface(defpath + "radar_background.png")));
    Vec2 offset = radarObject->transform()->position();
    radarObject->transform()->position(offset);
    radarObject = instantiate(radarObject);
    radarObject->transform()->set_parent(radar->transform(), false);
    radarObject->get_sprite_renderer()->set_sprite(ResourceManager::make_sprite(ResourceManager::GetSurface(defpath + "radar_foreground.png")));
    radar_foreground = radarObject->transform();
    create_game_object("Tail")->add_component<SpriteRenderer>();

    // Создаем N яблоко
    int n = 1000;
    int x;
    float range = maxWidth;
    GameObject* appleObject = create_game_object("apple");
    appleObject->transform()->layer = 1;
    SpriteRenderer* view2 = appleObject->add_component<SpriteRenderer>();
    view2->set_sprite(ResourceManager::make_sprite(appleTexture));
    view2->size = Vec2::half;
    apples.reserve(n + 1);
    apples.emplace_back(appleObject);
    for (x = 0; x < n; ++x) {
        appleObject = instantiate(appleObject);
        appleObject->transform()->position(Vec2(Random::range(-range, range), Random::range(-range, range)));
        apples.emplace_back(appleObject);
    }
}

int score = 0;
int target_n = 0;
float distance = 3;
void AppleEatGameLevel::on_update()
{

    int culled = this->get_culled();
    get_gui()->set_text(mids.text, "Render: " + std::to_string(culled));

    std::string temp;
    temp = "Score: ";
    temp += std::to_string(score);
    get_gui()->set_text(mids.text, temp);

    auto v = Camera::main_camera()->transform()->position();

    Camera::main_camera()->transform()->position(Vec2::lerp(v, player->transform()->position(), 9 * TimeEngine::deltaTime()));

    std::vector<Transform*> finded = Physics2D::sphere_cast<std::vector<Transform*>>(player->transform()->position(), distance, 1);
    for (int x = 0, y = Math::min<int>(128, finded.size()); x < y; ++x) {
        Transform* t;
        Transform* f = finded.at(x);
        if (f->game_object()->name() != "apple (clone)")
            continue;

        f->transform()->position(Vec2::move_towards(f->transform()->position(), player->transform()->position(), 15 * TimeEngine::deltaTime()));
        if (Vec2::distance_sqr(player->transform()->position(), f->position()) < 0.3f * 0.3f)
            t = f;
        else
            t = nullptr;
        // Draw point destroyer
        // Gizmos::DrawLine(f->position(), player->transform()->position());

        if (t) {
            /*
                Vec2 newPoint;
                Vec2 j = Camera::viewport_to_world(Vec2::zero);
                Vec2 k = Camera::viewport_to_world(Vec2::one);
                newPoint.x = Math::outside(Random::range((float)-maxWidth, (float)maxWidth), j.x, k.x);
                newPoint.y = Math::outside(Random::range((float)-maxWidth, (float)maxWidth), j.y, k.y);
                t->position(newPoint);*/
            t->game_object()->destroy();
            ++score;
        } else {
            ++target_n;
        }
    }

    static bool ___c = true;

    if (Input::is_mouse_up(3))
        ___c = !___c;

    for (int x = 0; ___c && x < 2; ++x) {
        Vec2 newPoint;
        Vec2 j = Camera::viewport_to_world(Vec2::zero);
        Vec2 k = Camera::viewport_to_world(Vec2::one);
        newPoint.x = Math::outside(Random::range((float)-maxWidth, (float)maxWidth), j.x, k.x);
        newPoint.y = Math::outside(Random::range((float)-maxWidth, (float)maxWidth), j.y, k.y);
        instantiate(apples[0], newPoint);
    }
    return;
    apples[0]->transform()->position(Camera2D::screen_to_world(Input::get_mouse_pointf()));

    if (apples.size() == 1)
        return;

    for (int i = 1; false && i < apples.size(); ++i) {
        apples[i]->transform()->position(Vec2(Random::range(-maxWidth, maxWidth), Random::range(-maxWidth, maxWidth)));
    }
}

void AppleEatGameLevel::on_gizmo()
{
    //    std::string tree = this->get_hierarchy_as_tree();

    //    std::ofstream ofs {"/tmp/tree_live.txt"};
    //    ofs << tree;
    //    ofs.close();

    //  Gizmos::draw_2D_world_space(Vec2::zero);

    Gizmos::set_color(0x88241dff);

    // draw line

    static float angle = 360;

    if (angle > 360)
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
