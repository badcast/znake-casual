#include "gamelevel.h"

#include <fstream>

GameLevel::GameLevel()
    : World("ZNake Game Level")
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

void GameLevel::OnStart()
{
    Vec2Int p;
    Resolution res = RoninSimulator::GetCurrentResolution();
    const std::string defpath = "./data/sprites/";
    p.x = res.width - 245;
    p.y = 64;
    // ui->Push_Edit("This is text", p);
    // p.y += 64;
    uiSlider = getGUI()->PushSlider(0.5f, p);
    p.y += 32;
    uiSliderAngle = getGUI()->PushSlider(0.5f, p);

    p.y += 32;
    selRndr = getGUI()->PushDropDown(std::list<std::string>({ "Исходный", "Центрированный" }), 0, p, index_changed);

    terrain = create_game_object("Test")->AddComponent<Terrain2D>();
    terrain->get_navmesh2D()->worldScale /= 2;

    spr = create_game_object()->AddComponent<SpriteRenderer>();
    spr->set_sprite(ResourceManager::make_sprite(ResourceManager::GetSurface(defpath + "snake-body.png")));
    spr->renderPresentMode = SpriteRenderPresentMode::Place;
    spr->renderOut = SpriteRenderOut::Origin;

    snakeplayer = create_game_object("Player")->AddComponent<SnakePlayer>();
    snakeplayer->terrain = terrain;
    snakeplayer->transform()->position(Vec2::right);
    auto _ = static_cast<Camera2D*>(Camera::main_camera());
    _->visibleGrids = true;
    _->visibleBorders = true;
    _->visibleObjects = true;

    return;

    auto ground = create_game_object("Ground")->AddComponent<SpriteRenderer>();
    ground->set_sprite(ResourceManager::make_sprite(ResourceManager::GetSurface(defpath + "concrete.jpg")));
    ground->size = Vec2::one * 12;
    ground->renderType = SpriteRenderType::Tile;
    ground->transform()->layer = -1;
}

void GameLevel::OnUpdate()
{
    Vec2 ms = Camera::ScreenToWorldPoint(Input::GetMousePointf());
    spr->size.y = getGUI()->SliderGetValue(uiSlider);

    spr->transform()->angle(getGUI()->SliderGetValue(uiSliderAngle) * 360);
}

void GameLevel::OnGizmos() { Gizmos::DrawNavMesh(terrain->get_navmesh2D()); }
