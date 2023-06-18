#pragma once
#include <ronin/framework.h>

using namespace RoninEngine;
using namespace RoninEngine::Runtime;
using namespace RoninEngine::AIPathFinder;
using namespace RoninEngine::UI;

class AppleEatGameLevel : public World
{
public:
    AppleEatGameLevel();
    void on_start();
    void on_update();
    void on_gizmo();
};

class ResourceManager
{

public:
    static SDL_Surface* GetSurface(const std::string& path)
    {
        resource_id res = Resources::load_surface(path, false);

        if (res == -1) {
            throw "Error";
        }

        return Resources::get_surface(res);
    }

    static Sprite* make_sprite(SDL_Surface* surf)
    {
        Sprite* spr = Primitive::create_empty_sprite2D();
        spr->set_surface(surf);
        return spr;
    }
};
