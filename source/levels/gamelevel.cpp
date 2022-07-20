#include "gamelevel.h"

#include <fstream>

GameLevel::GameLevel() : Level("ZNake Game Level") {}

Terrain2D *terrain;
SnakePlayer *snakeplayer;
void GameLevel::start() {
    terrain = CreateGameObject("Test")->addComponent<Terrain2D>();
    terrain->getNavMesh()->worldScale /= 2;
    snakeplayer = CreateGameObject("Player")->addComponent<SnakePlayer>();
    snakeplayer->terrain = terrain;

    auto ground = CreateGameObject("Ground")->addComponent<SpriteRenderer>();
    ground->setSpriteFromTextureToGC(GC::GetTexture("concrete"));
    ground->size = Vec2::one * 12;
    ground->renderType = SpriteRenderType::Tile;
    ground->zOrder = -1;
}

void GameLevel::update() {

}

void GameLevel::onDrawGizmos() {
    Gizmos::DrawNavMesh(terrain->getNavMesh());
}
