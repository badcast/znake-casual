#include <ronin/Transform.h>
#include "testlevel.h"

struct {
    uid quitButton;
    uid clickButton;
    uid restore = -1;
    uid text;
} mids;

typedef ResourceManager GC;
TestLevel::TestLevel()
    : Level("ZNake Game Level")
{
}
GUI* ui;
void callback_ui(uid id, void* userData)
{
    if (id == mids.quitButton)
        Application::requestQuit();
    else if (id == mids.clickButton) {
        Rect r = ui->get_rect(id);
        r.w = 200;
        ui->set_rect(id, r);
        auto damaged = Level::matrixCheckDamaged();
        guiInstance->set_text(id, "Damaged: " + std::to_string(damaged.size()));

        if (mids.restore == -1) {
            r.x += r.w;
            r.w = 100;
            mids.restore = guiInstance->Push_Button("Restore", r);
        }

        guiInstance->set_visible(mids.restore, true);

    } else if (id == mids.restore) {
        ui->set_visible(mids.restore, false);
        Level::matrix_restore();
    }
}

std::vector<GameObject*> apples_for;
Player* aPlayer;
void TestLevel::start()
{
    ui = this->ui;
    // create a menu
    Texture* appleTexture = GC::GetTexture("apple");
    if (true) {
        Rect t(0, 0, 100, 32);
        mids.quitButton = ui->Push_Button("Quit", t);
        t.x += t.x + t.w;
        mids.clickButton = ui->Push_Button("Check", t);
        t.x += t.x + t.w;

        t.w = t.h = 32;

        ui->Push_TextureStick(appleTexture, t);

        ui->register_callback(&callback_ui, nullptr);

        Resolution res = Application::getResolution();
        t.w = 200;
        t.x = res.width - t.w;
        mids.text = guiInstance->Push_Button("", t);
    }
    // instance games
    auto floorSrc = GC::GetSurface("concrete");
    auto snakeheadSrc = GC::GetSurface("snake-head");

    GameObject* floor = create_game_object("Floor");
    SpriteRenderer* view = floor->add_component<SpriteRenderer>();
    view->setSpriteFromTextureToGC(floorSrc);
    view->size = Vec2::one * 7;
    // view->renderType = SpriteRenderType::Tile;
    view->renderPresentMode = SpriteRenderPresentMode::Place;
    view->transform()->position(Vec2::infinity);

    GameObject* playerGameObject = CreateGameObject("Player");
    aPlayer = playerGameObject->add_component<Player>();
    // player->playerCamera->visibleObjects = true;  // show objects in level
    aPlayer->spriteRenderer->setSpriteFromTextureToGC(snakeheadSrc);
    aPlayer->spriteRenderer->size = Vec2::one * 0.5f;
    aPlayer->transform()->layer = 1;

    SpriteRenderer* tail = CreateGameObject("Tail")->addComponent<SpriteRenderer>();
    auto curSrc = GC::GetSurface("snake-tail");
    tail->setSpriteFromTextureToGC(curSrc);
    tail->size = aPlayer->spriteRenderer->size;
    tail->transform()->setParent(playerGameObject->transform());
    tail->transform()->position(Vec2::minusOne / 2);

    Transform* t = Instantiate(playerGameObject)->transform();
    t->position(Vec2::one);
    t->angle(135);
    t = Instantiate(playerGameObject)->transform();
    t->position(Vec2::minusOne);
    t->angle(360 - 135);
    t = Instantiate(playerGameObject)->transform();
    t->position(Vec2::up + Vec2::left);
    t->angle(270);
    t = Instantiate(playerGameObject)->transform();
    t->position(Vec2::down + Vec2::right);
    t->angle(90);

    // Создаем N яблоко
    int n = 50000;
    int x;
    float range = 50;
    GameObject* appleObject = CreateGameObject("apple");
    SpriteRenderer* view2 = appleObject->add_component<SpriteRenderer>();
    view2->setSpriteFromTextureToGC(GC::GetSurface("appleTexture"));
    apples_for.reserve(n + 1);
    apples_for.emplace_back(appleObject);
    for (x = 0; x < n; ++x) {
        appleObject = Instantiate(appleObject);
        appleObject->transform()->position(Vec2(Random::range(-range, range), Random::range(-range, range)));
        apples_for.emplace_back(appleObject);
    }
}

void TestLevel::update()
{
    char mv[100];

    int culled, full;
    Level::render_info(&culled, &full);
    guiInstance->set_text(mids.text, "Render: " + std::to_string(full - culled));

    if (TimeEngine::frame() % 30 == 0) {
        auto xx = matrixCheckDamaged().size();
        guiInstance->set_text(mids.clickButton, "Damaged " + std::to_string(xx));
    }
    auto cmpnt = aPlayer->gameObject()->get_components<SpriteRenderer>();
    // cmpnt.back()->offsetFromWorldPosition(Camera2D::ScreenToWorldPoint(input::getMousePointF()));

    return;
    apples_for[0]->transform()->position(Camera2D::ScreenToWorldPoint(input::getMousePointF()));

    if (apples_for.size() == 1)
        return;

    for (int i = 1; false && i < apples_for.size(); ++i) {
        apples_for[i]->transform()->position(Vec2(Random::range(-10, 10), Random::range(-10, 10)));
    }
}

void TestLevel::onDrawGizmos()
{
    Gizmos::Draw2DWorldSpace(Vec2::zero);

    float distance = 3;
    Gizmos::setColor(0x88241dff);

    Gizmos::DrawCircle(aPlayer->transform()->position(), distance);

    auto finded = Physics2D::sphereCast(aPlayer->transform()->position(), distance);

    Transform* t = nullptr;
    for (Transform* f : finded) {
        if (f->gameObject()->name() != "apple (clone)")
            continue;

        f->transform()->position(Vec2::MoveTowards(f->transform()->position(), aPlayer->transform()->position(), 0.01f));
        if (!t && Vec2::Distance(aPlayer->transform()->position(), f->position()) < 0.3f)
            t = f;
        Gizmos::DrawLine(f->position(), aPlayer->transform()->position());
    }

    if (t) {
        t->position(Vec2::infinity);
    }
    return;
    Gizmos::DrawStorm(Vec2::zero, Math::number(Math::ceil(distance)));
}
