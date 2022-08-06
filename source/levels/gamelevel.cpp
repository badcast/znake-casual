#include "gamelevel.h"

#include <fstream>

GameLevel::GameLevel()
    : Level("ZNake Game Level")
{
}

void index_changed(uid id, int index)
{
    // do something
}

Terrain2D* terrain;
SnakePlayer* snakeplayer;
SpriteRenderer* spr;

uid uiSliderAngle;
uid uiSlider;
uid selRndr;

void GameLevel::start()
{
    Vec2Int p;
    p.x = Application::getResolution().width - 245;
    p.y = 64;
    // ui->Push_Edit("This is text", p);
    // p.y += 64;
    uiSlider = ui->Push_Slider(0.5f, p);

    p.y += 32;
    uiSliderAngle = ui->Push_Slider(0.5f, p);

    p.y += 32;
    selRndr = ui->Push_DropDown(std::list<std::string>({ "Исходный", "Центрированный" }), 0, p, (event_index_changed*)&index_changed);

    terrain = create_game_object("Test")->add_component<Terrain2D>();
    terrain->surfaceMesh()->worldScale /= 2;

    spr = create_game_object()->add_component<SpriteRenderer>();
    spr->setSpriteFromTextureToGC(ResourceManager::GetSurface("snake-body"));
    spr->renderPresentMode = SpriteRenderPresentMode::Place;
    spr->renderOut = SpriteRenderOut::Origin;

    snakeplayer = CreateGameObject("Player")->addComponent<SnakePlayer>();
    snakeplayer->terrain = terrain;
    snakeplayer->transform()->position(Vec2::right);
    auto _ = ((Camera2D*)Camera::mainCamera());
    _->visibleGrids = true;
    _->visibleBorders = true;
    _->visibleObjects = true;
    return;

    auto ground = CreateGameObject("Ground")->addComponent<SpriteRenderer>();
    ground->setSpriteFromTextureToGC(GC::GetSurface("concrete"));
    ground->size = Vec2::one * 12;
    ground->renderType = SpriteRenderType::Tile;
    ground->transform()->layer = -1;
}

void GameLevel::update()
{
    Vec2 ms = Camera::ScreenToWorldPoint(input::getMousePointF());
    spr->size.y = *(float*)ui->getResources(uiSlider);

    spr->transform()->angle(*(float*)ui->getResources(uiSliderAngle) * 360);
}

void GameLevel::onDrawGizmos() { Gizmos::DrawNavMesh(terrain->surfaceMesh()); }
