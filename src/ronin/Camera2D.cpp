#include "framework.h"

namespace RoninEngine::Runtime {
Camera2D::Camera2D() : Camera("Camera 2D"), scale(Vec2::one) {
    this->visibleBorders = false;
    this->visibleGrids = false;
    this->visibleObjects = false;
}

RoninEngine::Runtime::Camera2D::Camera2D(const Camera2D& proto)
    : Camera(proto.m_name),
      visibleBorders(proto.visibleBorders),
      visibleGrids(proto.visibleGrids),
      visibleObjects(proto.visibleObjects),
      scale(proto.scale) {}

void Camera2D::render(SDL_Renderer* renderer, Rect rect, GameObject* root) {
    Render_info renderInfo;
    Rectf_t dst;
    Vec2* point;
    Vec2* sourcePoint;
    //Vec2 _scale;

    Gizmos::color = Color(0xc4c4c4);

    if (visibleGrids) {
        Gizmos::Draw2DWorldSpace(Vec2::zero);
        Gizmos::DrawPosition(transform()->position());
    }

    auto stack = matrixSelection();
    //scale.x = Mathf::Min(Mathf::Max(scale.x, 0.f), 10.f);
    //scale.y = Mathf::Min(Mathf::Max(scale.y, 0.f), 10.f);
    //_scale = scale*squarePerPixels;
    SDL_RenderSetScale(renderer, scale.x, scale.y);
    // Render Objects
    for (auto renderSource : *std::get<std::set<Renderer*>*>(stack)) {
        // drawing
        // clear
        renderInfo = {};
        renderInfo.renderer = renderer;

        renderSource->Render(&renderInfo);  // draw

        if (renderInfo.texture) {
            Vec2* point = &transform()->_p;
            Vec2* sourcePoint = &renderSource->transform()->_p;

            dst.w = renderInfo.dst.w * squarePerPixels;//_scale.x;
            dst.h = renderInfo.dst.h * squarePerPixels;//_scale.y;

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
    for (auto lightSource : *std::get<std::set<Light*>*>(stack)) {
        // drawing
        // clear
        renderInfo = {};
        renderInfo.renderer = renderer;

        lightSource->GetLightSource(&renderInfo);  // draw

        if (renderInfo.texture) {
            point = &transform()->_p;
            sourcePoint = &lightSource->transform()->_p;

            dst.w = renderInfo.dst.w * squarePerPixels;;
            dst.h = renderInfo.dst.h * squarePerPixels;;

            // h
            dst.x = ((rect.w - dst.w) / 2.0f - (point->x - sourcePoint->x) * squarePerPixels);
            // v
            dst.y = ((rect.h - dst.h) / 2.0f + (point->y - sourcePoint->y) * squarePerPixels);

            SDL_RenderCopyExF(renderer, renderInfo.texture->native(), (SDL_Rect*)&renderInfo.src, (SDL_FRect*)&dst,
                              lightSource->transform()->_angle - transform()->_angle, nullptr, SDL_RendererFlip::SDL_FLIP_NONE);
        }
    }


    if (visibleBorders) {
        float offset = 25 * std::max(1 - Time::deltaTime(), 0.5f);
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

    if (visibleObjects) {
        for (auto face : (*std::get<std::set<Renderer*>*>(stack))) {
            Vec2 p = face->transform()->position();
            Vec2 sz = face->GetSize() * 2;
            Gizmos::color = Color::blue;
            Gizmos::DrawRectangleRounded(p, sz.x, sz.y, 5);
            Gizmos::color = Color::black;
            Gizmos::DrawPosition(p, 0.2f);
            Gizmos::DrawTextOnPosition(p, face->gameObject()->m_name);
        }
    }
}
}  // namespace RoninEngine::Runtime
