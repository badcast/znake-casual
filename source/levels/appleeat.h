#pragma once
#include <ronin/framework.h>

using namespace RoninEngine;
using namespace RoninEngine::Runtime;
using namespace RoninEngine::AI;
using namespace RoninEngine::UI;

class AppleEatGameLevel : public World
{
public:
    AppleEatGameLevel();
    void OnStart();
    void OnUpdate();
    void OnGizmos();
};

class ResourceManager
{

public:
    static SDL_Surface* GetSurface(const std::string& path)
    {
        ResId res = Resources::LoadImage(path, true);

        if (res == -1) {
            throw "Error";
        }

        return Resources::GetImageSource(res);
    }

    static SpriteRef make_sprite(SDL_Surface* surf)
    {
        SpriteRef spr = Primitive::CreateEmptySprite();
        spr->setSurface(surf);
        return spr;
    }
};
