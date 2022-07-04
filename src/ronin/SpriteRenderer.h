#pragma once

#include "dependency.h"

namespace RoninEngine::Runtime {
enum class SpriteRenderTile : char { Fixed, Place };
enum class SpriteRenderType : char {
    ///Обычное отрисовка спрайта с учетом его параметров.
    Simple,
    ///Отрисовка спрайта с учетом его параметров и черепиция от size
    Tile
};

class SpriteRenderer : public Renderer {
    Texture* texture;
    Sprite* sprite;

   public:
    virtual ~SpriteRenderer();

    SpriteRenderType renderType;
    SpriteRenderTile tileRenderPresent;
    Color color;
    Vec2 size;
    Vec2 flip;

    SpriteRenderer();
    SpriteRenderer(const std::string& name);
    SpriteRenderer(const SpriteRenderer&);

    Vec2 GetSize();

    void setSprite(Sprite* sprite);
    void setSpriteFromTextureToGC(Texture* texture);
    Sprite* getSprite();

    void Render(Render_info* render_info);
};

}  // namespace RoninEngine::Runtime
