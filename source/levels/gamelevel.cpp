#include "gamelevel.h"

GameLevel::GameLevel()
    : World("ZNake Game Level")
{
}

void GameLevel::OnStart()
{
    Vec2Int p;
    const std::string defpath = "./data/sprites/";

    terrain = Primitive::CreateEmptyGameObject("Test")->AddComponent<Terrain2D>();
    terrain->get_navmesh2D()->worldScale *= 0.64f;

    snakeplayer = Primitive::CreateEmptyGameObject("Player")->AddComponent<SnakePlayer>();
    snakeplayer->terrain = terrain;
    snakeplayer->transform()->position(Vec2::right);

    auto ground = Primitive::CreateEmptyGameObject("Ground")->AddComponent<SpriteRenderer>();
    ground->setSprite(ResourceManager::make_sprite(ResourceManager::GetSurface(defpath + "concrete.jpg")));
    ground->setPresentMode(SpriteRenderPresentMode::Place);
    ground->setSize(Vec2::one * 200);
    ground->setRenderType(SpriteRenderType::Simple);
    ground->transform()->layer(-1);
}

