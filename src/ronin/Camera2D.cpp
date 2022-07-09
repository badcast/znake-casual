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
    Render_info wrapper;
    Vec2* point;
    Vec2 sourcePoint;
    // Vec2 _scale;

    Gizmos::color = 0xc4c4c4;

    if (visibleGrids) {
        Gizmos::Draw2DWorldSpace(Vec2::zero);
        Gizmos::DrawPosition(transform()->position());
    }

    auto stack = matrixSelection();
    // scale.x = Mathf::Min(Mathf::Max(scale.x, 0.f), 10.f);
    // scale.y = Mathf::Min(Mathf::Max(scale.y, 0.f), 10.f);
    //_scale = scale*squarePerPixels;
    SDL_RenderSetScale(renderer, scale.x, scale.y);
    // Render Objects
    for (auto renderSource : *std::get<std::set<Renderer*>*>(stack)) {
        // drawing
        // clear
        wrapper = {};
        wrapper.renderer = renderer;

        renderSource->Render(&wrapper);  // draw
        if (wrapper.texture) {
            Vec2 point = transform()->p;
            Transform* rTrans = renderSource->transform();

            if (rTrans->m_parent && renderSource->transform()->m_parent != Level::self()->main_object->transform()) {
                // TODO: Сделать возможность отрисовки вращений, дочерных элментов. Внимание не рабочая область
                // WARNING: Не рабочая область кода, не следует добавлять Transform к SetParent
                // throw std::runtime_error("This method not implemented");

                /*
                где localPosition это изначальный вектор смешения объекта относительно center
                ScreenToWorldPoint(input::getMousePointF())
                Vec2::one*0.3f
                */
                Vec2 locl = rTrans->localPosition();
                Vec2 r = Vec2::RotateAround(ScreenToWorldPoint(input::getMousePointF()), rTrans->p.normalized(), rTrans->angle() * Mathf::Deg2Rad);
               // rTrans->position(r);
                sourcePoint = r;
            }
            else {
                 sourcePoint = rTrans->position();
            }

            wrapper.dst.w *= pixelsPerSize;  //_scale.x;
            wrapper.dst.h *= pixelsPerSize;  //_scale.y;

            Vec2 arranged(wrapper.dst.x, wrapper.dst.y);

            /*
                        ((позиция тела).x * sin(угла поворота тела), (позиция тела).y * cos(угла поворота тела)) + (позиция
               тела)
            */

            // Vec2 rotated = arranged=Vec2::RotateAround(sourcePoint, arranged,
            // renderSource->transform()->localAngle()*Mathf::Deg2Rad);

            // arranged.x -= rotated.x;
            // arranged.y += rotated.y;


            //Положение по горизонтале
            wrapper.dst.x = arranged.x + ((rect.w - wrapper.dst.w) / 2.0f - (point.x - sourcePoint.x) * pixelsPerSize);
            //Положение по вертикале
            wrapper.dst.y = arranged.y + ((rect.h - wrapper.dst.h) / 2.0f + (point.y - sourcePoint.y) * pixelsPerSize);

            SDL_RenderCopyExF(renderer, wrapper.texture->native(), (SDL_Rect*)&wrapper.src,
                              reinterpret_cast<SDL_FRect*>(&wrapper.dst), renderSource->transform()->angle(), nullptr,
                              SDL_RendererFlip::SDL_FLIP_NONE);
        }
    }
    // Render Lights
    for (auto lightSource : *std::get<std::set<Light*>*>(stack)) {
        // drawing
        // clear
        wrapper = {};
        wrapper.renderer = renderer;

        lightSource->GetLightSource(&wrapper);  // draw

        if (wrapper.texture) {
            point = &transform()->p;
            sourcePoint = lightSource->transform()->position();

            wrapper.dst.w *= pixelsPerSize;

            wrapper.dst.h *= pixelsPerSize;

            // h
            wrapper.dst.x += ((rect.w - wrapper.dst.w) / 2.0f - (point->x - sourcePoint.x) * pixelsPerSize);
            // v
            wrapper.dst.y += ((rect.h - wrapper.dst.h) / 2.0f + (point->y - sourcePoint.y) * pixelsPerSize);

            SDL_RenderCopyExF(renderer, wrapper.texture->native(), reinterpret_cast<SDL_Rect*>(&wrapper.src),
                              reinterpret_cast<SDL_FRect*>(&wrapper.dst),
                              lightSource->transform()->_angle - transform()->_angle, nullptr, SDL_RendererFlip::SDL_FLIP_NONE);
        }
    }

    if (visibleBorders) {
        float offset = 25 * std::max(1 - Time::deltaTime(), 0.5f);
        float height = 200 * Time::deltaTime();

        wrapper.dst.x = ((rect.w) / 2.0f);
        wrapper.dst.y = ((rect.h) / 2.0f);

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 25);

        // Center dot
        SDL_RenderDrawPointF(renderer, wrapper.dst.x, wrapper.dst.y);
        SDL_RenderDrawPointF(renderer, wrapper.dst.x - offset, wrapper.dst.y);
        SDL_RenderDrawPointF(renderer, wrapper.dst.x + offset, wrapper.dst.y);
        SDL_RenderDrawPointF(renderer, wrapper.dst.x, wrapper.dst.y - offset);
        SDL_RenderDrawPointF(renderer, wrapper.dst.x, wrapper.dst.y + offset);

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
