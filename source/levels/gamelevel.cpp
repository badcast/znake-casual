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

void GameLevel::on_start()
{
    Vec2Int p;
    Resolution res = RoninSimulator::get_current_resolution();
    const std::string defpath = "./data/sprites/";
    p.x = res.width - 245;
    p.y = 64;
    // ui->Push_Edit("This is text", p);
    // p.y += 64;
    uiSlider = get_gui()->push_slider(0.5f, p);
    p.y += 32;
    uiSliderAngle = get_gui()->push_slider(0.5f, p);

    p.y += 32;
    selRndr = get_gui()->push_drop_down(std::list<std::string>({ "Исходный", "Центрированный" }), 0, p, index_changed);

    terrain = create_game_object("Test")->add_component<Terrain2D>();
    terrain->get_navmesh2D()->worldScale /= 2;

    spr = create_game_object()->add_component<SpriteRenderer>();
    spr->set_sprite(ResourceManager::make_sprite(ResourceManager::GetSurface(defpath + "snake-body.png")));
    spr->renderPresentMode = SpriteRenderPresentMode::Place;
    spr->renderOut = SpriteRenderOut::Origin;

    snakeplayer = create_game_object("Player")->add_component<SnakePlayer>();
    snakeplayer->terrain = terrain;
    snakeplayer->transform()->position(Vec2::right);
    auto _ = static_cast<Camera2D*>(Camera::main_camera());
    _->visibleGrids = true;
    _->visibleBorders = true;
    _->visibleObjects = true;

    return;

    auto ground = create_game_object("Ground")->add_component<SpriteRenderer>();
    ground->set_sprite(ResourceManager::make_sprite(ResourceManager::GetSurface(defpath + "concrete.jpg")));
    ground->size = Vec2::one * 12;
    ground->renderType = SpriteRenderType::Tile;
    ground->transform()->layer = -1;
}

void GameLevel::on_update()
{
    Vec2 ms = Camera::screen_to_world(Input::get_mouse_pointf());
    spr->size.y = get_gui()->get_slider_value(uiSlider);

    spr->transform()->angle(get_gui()->get_slider_value(uiSliderAngle) * 360);
}

void GameLevel::on_gizmo() { Gizmos::draw_nav_mesh(terrain->get_navmesh2D()); }
