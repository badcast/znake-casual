#include "gamelevel.h"

#include <fstream>

GameLevel::GameLevel() : Level("ZNake Game Level") {}

Terrain2D *terrain;
SnakePlayer *snakeplayer;
SpriteRenderer *spr;
void GameLevel::start() {
    Vec2Int p;
    p.x = Application::getResolution().width - 240;
    p.y = 0;
    std::vector<std::string> el1 = {"Элемент 1", "Элемент 2"};
    ui->Push_DropDown(el1, p);
    p.y += 64;

    ui->Push_Edit("This is text", p);

    terrain = CreateGameObject("Test")->addComponent<Terrain2D>();
    terrain->getNavMesh()->worldScale /= 2;

    spr = CreateGameObject()->addComponent<SpriteRenderer>();
    spr->setSpriteFromTextureToGC(GC::GetTexture("snake-body"));
    spr->renderPresentMode = SpriteRenderPresentMode::Place;
    spr->renderOut = SpriteRenderOut::Origin;

    snakeplayer = CreateGameObject("Player")->addComponent<SnakePlayer>();
    snakeplayer->terrain = terrain;
    snakeplayer->transform()->position(Vec2::right * 2);
    auto _ = ((Camera2D *)Camera::mainCamera());
    _->visibleGrids = true;
    _->visibleBorders = true;
    _->visibleObjects = true;
    return;

    auto ground = CreateGameObject("Ground")->addComponent<SpriteRenderer>();
    ground->setSpriteFromTextureToGC(GC::GetTexture("concrete"));
    ground->size = Vec2::one * 12;
    ground->renderType = SpriteRenderType::Tile;
    ground->transform()->layer = -1;
}

void GameLevel::update() {
    Vec2 ms = Camera::ScreenToWorldPoint(input::getMousePointF());
    spr->size.y = Math::Clamp01(-ms.y);
}

void GameLevel::onDrawGizmos() { Gizmos::DrawNavMesh(terrain->getNavMesh()); }
