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
int createWith = 10;
int createStart = 5000;

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
        Rect rect = gui_instance->ElementGetRect(id);
        rect.w = 200;
        gui_instance->ElementSetRect(id, rect);

        if(mids.restore == -1)
        {
            rect.x += rect.w;
            rect.w = 100;
            mids.restore = gui_instance->PushButton("Restore", rect);
        }

        gui_instance->ElementSetVisible(mids.restore, true);
    }
    else if(id == mids.restore)
    {
        gui_instance->ElementSetVisible(mids.restore, false);
    }
    else if(id == mids.reloadButton)
    {
        RoninSimulator::ReloadWorld();
    }
}

std::vector<GameObjectRef> apples;
MoveController2DRef player;

TransformRef radar_foreground;
void AppleEatGameLevel::OnStart()
{
    apples = {};
    gui_instance = GetGUI();
    const std::string defpath = "./data/sprites/";
    // create a menu
    auto appleTexture = ResourceManager::GetSurface(defpath + "apple.png");
    Rect t(0, 0, 100, 32);
    mids.quitButton = GetGUI()->PushButton("Quit", t);
    float y = t.y;
    t.y += t.y + t.h;
    mids.reloadButton = GetGUI()->PushButton("Reload world", t);
    t.y = y;
    t.x += t.x + t.w;
    mids.clickButton = GetGUI()->PushButton("Check", t);
    t.x += t.x + t.w;

    t.w = t.h = 32;

    // guiInstance->Push_TextureStick(appleTexture, t);

    GetGUI()->SetGeneralCallback(callback, nullptr);

    Resolution res = RoninSimulator::GetCurrentResolution();
    t.w = 200;
    t.x = res.width - t.w;
    mids.text = GetGUI()->PushButton("", t);
    t.y += 32;
    // instance games
    auto floorTexture = ResourceManager::GetSurface(defpath + "concrete.jpg");

    GameObjectRef floor = Primitive::CreateEmptyGameObject("Floor");
    SpriteRendererRef view = floor->AddComponent<SpriteRenderer>();
    view->setSprite(ResourceManager::make_sprite(floorTexture));
    view->setSize(Vec2::one * 7);
    // view->renderType = SpriteRenderType::Tile;
    view->setPresentMode(SpriteRenderPresentMode::Place);
    floor->transform()->layer(-100);
    //  view->transform()->position(Vec2::infinity);

    GameObjectRef playerGameObject = Primitive::CreateEmptyGameObject("Player");
    player = playerGameObject->AddComponent<MoveController2D>();
    player->AddComponent<SpriteRenderer>();
    player->spriteRenderer()->transform()->layer(100);

    GameObjectRef radarObject = Primitive::CreateEmptyGameObject("Radar");
    radarObject->transform()->setParent(player->transform());
    SpriteRendererRef radar = radarObject->AddComponent<SpriteRenderer>();
    // radar->set_sprite(ResourceManager::make_sprite(ResourceManager::GetSurface(defpath + "radar_background.png")));
    Vec2 offset = radarObject->transform()->position();
    radarObject->transform()->position(offset);
    radarObject = Instantiate(radarObject);
    radarObject->transform()->setParent(radar->transform(), false);
    radarObject->spriteRenderer()->setSprite(ResourceManager::make_sprite(ResourceManager::GetSurface(defpath + "radar_foreground.png")));
    radar_foreground = radarObject->transform();
    Primitive::CreateEmptyGameObject("Tail")->AddComponent<SpriteRenderer>();

    // Создаем N яблоко
    int n = createStart;
    int x;
    float range = maxWidth;
    GameObjectRef appleObject = Primitive::CreateEmptyGameObject("apple");
    appleObject->transform()->layer(1);
    SpriteRendererRef view2 = appleObject->AddComponent<SpriteRenderer>();
    view2->setSprite(ResourceManager::make_sprite(appleTexture));
    view2->setSize(Vec2::half);
    apples.emplace_back(appleObject);
    for(x = 0; x < n; ++x)
    {
        appleObject = Instantiate(appleObject);
        appleObject->transform()->position(Vec2(Random::Range(-range, range), Random::Range(-range, range)));
        //apples.emplace_back(appleObject);
    }
}

int score = 0;
int target_n = 0;
float distance = 3;
void AppleEatGameLevel::OnUpdate()
{
    int culled = this->GetCulled();
    GetGUI()->ElementSetText(mids.text, "Render: " + std::to_string(culled));

    std::string temp;
    temp = "Score: ";
    temp += std::to_string(score);
    GetGUI()->ElementSetText(mids.text, temp);

    Vec2 cameraPosition = Camera::mainCamera()->transform()->position();

    Camera::mainCamera()->transform()->position(Vec2::Lerp(cameraPosition, player->transform()->position(), 9 * Time::deltaTime()));

    std::vector<Transform *> finded = Physics2D::GetCircleCast<std::vector<Transform *>>(player->transform()->position(), distance, 0);
    for(int x = 0, y = Math::Min<int>(128, finded.size()); x < y; ++x)
    {
        Transform *t;
        Transform *f = finded.at(x);
        if(f->gameObject()->name() != "apple (clone)")
            continue;

        f->transform()->position(
            Vec2::MoveTowards(f->transform()->position(), player->transform()->position(), 15 * Time::deltaTime()));
        if(Vec2::DistanceSqr(player->transform()->position(), f->position()) < 0.3f * 0.3f)
            t = f;
        else
            t = nullptr;
        // Draw point destroyer
        // RenderUtility::DrawLine(f->position(), player->transform()->position());

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

    static bool ___c = false;

    if(Input::GetMouseUp(MouseButton::MouseMiddle))
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
    RenderUtility::SetColor(0x88241dff);

    // draw line

    static float angle = 360;

    if(angle > 360)
        angle -= 360;

    Vec2 sidePoint = player->transform()->position();

    sidePoint.x += Math::Cos(angle * Math::deg2rad) * distance;
    sidePoint.y += Math::Sin(angle * Math::deg2rad) * distance;
    radar_foreground->transform()->angle(angle);
    angle += 9;

    RenderUtility::DrawTextLegacy(Camera::ViewportToWorldPoint(Vec2::up / 2), "Apples: " + std::to_string(this->GetCulled()));
}
