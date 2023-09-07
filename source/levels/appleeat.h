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
        resource_id res = Resources::LoadImage(path, false);

        if (res == -1) {
            throw "Error";
        }

        return Resources::GetImageSource(res);
    }

    static Sprite* make_sprite(SDL_Surface* surf)
    {
        Sprite* spr = Primitive::create_empty_sprite2D();
        spr->setSurface(surf);
        return spr;
    }
};
