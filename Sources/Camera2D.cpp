#include "Camera2D.h"

#include "GameObject.h"
#include "Gizmos.h"
#include "Sprite.h"
#include "SpriteRenderer.h"
#include "inputSystem.h"
#include "pch.h"

namespace RoninEngine::Runtime {
Camera2D::Camera2D() : Camera("Camera 2D") {
    this->visibleBorders = false;
    this->visibleGrids = false;
}

Camera2D::Camera2D(const Camera2D& ref) {
    this->name() = ref.name();
    this->visibleBorders = ref.visibleBorders;
    this->visibleGrids = ref.visibleBorders;
}

void Camera2D::render(SDL_Renderer* renderer, Rect_t rect, GameObject* root) {
    Render_info renderInfo;
    Rectf_t dst;

    if (visibleGrids) {
        Gizmos::Draw2DWorldSpace(Vec2::zero);
        Gizmos::color.r = 128;
        Gizmos::DrawPosition(transform()->position());
    }

    auto stack = GetRenderersOfScreen();
    SDL_RenderSetScale(renderer, aspectRatio.x, aspectRatio.y);
    // Render Objects
    for (auto renderSource : *std::get<std::list<Renderer*>*>(stack)) {
        // drawing
        // clear
        renderInfo = {};
        renderInfo.renderer = renderer;

        renderSource->Render(&renderInfo);  // draw

        if (renderInfo.texture) {
            Vec2* point = &transform()->_p;
            Vec2* sourcePoint = &renderSource->transform()->_p;

            dst.w = renderInfo.dst.w * squarePerPixels;
            dst.h = renderInfo.dst.h * squarePerPixels;

            //Положение по горизонтале
            dst.x = ((rect.w - dst.w) / 2.0f - (point->x - sourcePoint->x) * squarePerPixels);
            //Положение по вертикале
            dst.y = ((rect.h - dst.h) / 2.0f + (point->y - sourcePoint->y) * squarePerPixels);

            // SDL_RenderCopyF(renderer, renderInfo.texture, (SDL_Rect*)&renderInfo.src, (SDL_FRect*)&dst);
            SDL_RenderCopyExF(renderer, renderInfo.texture->native(), (SDL_Rect*)&renderInfo.src, (SDL_FRect*)&dst,
                              renderSource->transform()->_angle, nullptr, SDL_RendererFlip::SDL_FLIP_NONE);
        }
    }
    // Render Lights
    for (auto lightSource : *std::get<std::list<Light*>*>(stack)) {
        // drawing
        // clear
        renderInfo = {};
        renderInfo.renderer = renderer;

        lightSource->GetLightSource(&renderInfo);  // draw

        if (renderInfo.texture) {
            Vec2* point = &transform()->_p;
            Vec2* sourcePoint = &lightSource->transform()->_p;

            dst.w = renderInfo.dst.w * squarePerPixels;
            dst.h = renderInfo.dst.h * squarePerPixels;

            // h
            dst.x = ((rect.w - dst.w) / 2.0f - (point->x - sourcePoint->x) * squarePerPixels);
            // v
            dst.y = ((rect.h - dst.h) / 2.0f + (point->y - sourcePoint->y) * squarePerPixels);

            // SDL_RenderCopyF(renderer, renderInfo.texture, (SDL_Rect*)&renderInfo.src, (SDL_FRect*)&dst);
            SDL_RenderCopyExF(renderer, renderInfo.texture->native(), (SDL_Rect*)&renderInfo.src, (SDL_FRect*)&dst,
                              lightSource->transform()->_angle - transform()->_angle, nullptr, SDL_RendererFlip::SDL_FLIP_NONE);
        }
    }

    SDL_RenderSetScale(renderer, 1, 1);
    if (visibleBorders) {
        float offset = 25 * max(1 - Time::deltaTime(), 0.5f);
        float height = 200 * Time::deltaTime();

        renderInfo.dst.x = ((rect.w) / 2.0f);
        renderInfo.dst.y = ((rect.h) / 2.0f);

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 25);

        // Center dot
        SDL_RenderDrawPointF(renderer, renderInfo.dst.x, renderInfo.dst.y);
        SDL_RenderDrawPointF(renderer, renderInfo.dst.x - offset, renderInfo.dst.y);
        SDL_RenderDrawPointF(renderer, renderInfo.dst.x + offset, renderInfo.dst.y);
        SDL_RenderDrawPointF(renderer, renderInfo.dst.x, renderInfo.dst.y - offset);
        SDL_RenderDrawPointF(renderer, renderInfo.dst.x, renderInfo.dst.y + offset);

        // borders
        SDL_RenderDrawLine(renderer, rect.x + offset, rect.y + offset, rect.x + offset + height, rect.y + offset);
        SDL_RenderDrawLine(renderer, rect.w - offset, rect.y + offset, rect.w - offset - height, rect.y + offset);
        SDL_RenderDrawLine(renderer, rect.x + offset, rect.h - offset, rect.x + offset + height, rect.h - offset);
        SDL_RenderDrawLine(renderer, rect.w - offset, rect.h - offset, rect.w - offset - height, rect.h - offset);

        SDL_RenderDrawLine(renderer, rect.x + offset, rect.y + 1 + offset, rect.x + offset, rect.y + offset + height);
        SDL_RenderDrawLine(renderer, rect.w - offset, rect.y + 1 + offset, rect.w - offset, rect.y + offset + height);
        SDL_RenderDrawLine(renderer, rect.x + offset, rect.h - 1 - offset, rect.x + offset, rect.h - offset - height);
        SDL_RenderDrawLine(renderer, rect.w - offset, rect.h - 1 - offset, rect.w - offset, rect.h - offset - height);
    }
}
}  // namespace RoninEngine::Runtime
