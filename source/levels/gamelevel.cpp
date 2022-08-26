#include "gamelevel.h"

#include <fstream>

GameLevel::GameLevel() : Level("ZNake Game Level") {}

Terrain2D *terrain;
SnakePlayer *snakeplayer;
void GameLevel::start() {
    std::vector<float> coins = {1, 2, 6, 12, 24, 120};
    Vec2Int p;
    p.x = 900;
    p.y = 40;
    ui->Push_DropDown(coins, p);
    p.y += 40;
    std::vector<std::string> el1 = {"Элемент 1", "Элемент 2"};
    ui->Push_DropDown(el1, p);

    terrain = CreateGameObject("Test")->addComponent<Terrain2D>();
    terrain->getNavMesh()->worldScale /= 2;
    snakeplayer = CreateGameObject("Player")->addComponent<SnakePlayer>();
    snakeplayer->terrain = terrain;

    auto ground = CreateGameObject("Ground")->addComponent<SpriteRenderer>();
    ground->setSpriteFromTextureToGC(GC::GetTexture("concrete"));
    ground->size = Vec2::one * 12;
    ground->renderType = SpriteRenderType::Tile;
    ground->transform()->layer = -1;
}

void GameLevel::update() {}

void GameLevel::onDrawGizmos() { Gizmos::DrawNavMesh(terrain->getNavMesh()); }
