#include "Gizmos.h"

#include <SDL2/SDL2_gfxPrimitives.h>

#include "framework.h"

namespace RoninEngine::Runtime {

Color Gizmos::color;
float Gizmos::angle;

void Gizmos::DrawLine(Vec2 a, Vec2 b) {
    if (!Camera::mainCamera()) return;

    Vec2 p = Camera::mainCamera()->transform()->position();
    Vec2 dst;

    auto res = Application::getResolution();

    // dst.x = ((rect.w - dst.w) / 2.0f - (point->x + sourcePoint->x) *
    // squarePerPixels); dst.y = ((rect.h - dst.h) / 2.0f + (point->y -
    // sourcePoint->y) * squarePerPixels);

    // scalars

    Vec2 scale;
    SDL_RenderGetScale(Application::GetRenderer(), &scale.x, &scale.y);
    scale *= pixelsPerSize;
    dst.x = res.width / 2.f;
    dst.y = res.height / 2.f;
    a.x = dst.x - (p.x - a.x) * scale.x;
    a.y = dst.y + (p.y - a.y) * scale.y;
    b.x = dst.x - (p.x - b.x) * scale.x;
    b.y = dst.y + (p.y - b.y) * scale.y;

    SDL_SetRenderDrawColor(Application::GetRenderer(), color.r, color.g, color.b, color.a);
    SDL_RenderDrawLineF(Application::GetRenderer(), a.x, a.y, b.x, b.y);
}
void drawBox() {
    Vec2 a, b;

    a.x = pixelsPerSize;
    a.y = pixelsPerSize;
    b.x = -pixelsPerSize;
    b.y = pixelsPerSize;
    Gizmos::DrawLine(std::move(a), std::move(b));

    a.x = pixelsPerSize;
    a.y = -pixelsPerSize;
    b.x = -pixelsPerSize;
    b.y = -pixelsPerSize;
    Gizmos::DrawLine(std::move(a), std::move(b));

    a.x = -pixelsPerSize;
    a.y = pixelsPerSize;
    b.x = -pixelsPerSize;
    b.y = -pixelsPerSize;
    Gizmos::DrawLine(std::move(a), std::move(b));

    a.x = pixelsPerSize;
    a.y = pixelsPerSize;
    b.x = pixelsPerSize;
    b.y = -pixelsPerSize;
    Gizmos::DrawLine(std::move(a), std::move(b));
}

void Gizmos::DrawPosition(const Vec2& origin, float scalar) {
    Vec2 a = Vec2::zero, b = Vec2::zero;

    // Draw Line H
    b = a = origin;
    a.x -= scalar;
    b.x += scalar;
    DrawLine(std::move(a), std::move(b));

    // Draw Line V
    b = a = origin;
    a.y -= scalar;
    b.y += scalar;
    DrawLine(std::move(a), std::move(b));
}

void Gizmos::DrawSquare(Vec2 origin, float width) { DrawRectangle(origin, width, width); }

void Gizmos::DrawRectangle(Vec2 origin, float width, float height) {
    origin = Camera::mainCamera()->WorldToScreenPoint(origin);
    std::uint16_t x, y;
    width *= pixelsPerSize;
    height *= pixelsPerSize;
    x = origin.x - width / 2;
    y = origin.y - height / 2;

    rectangleColor(Application::GetRenderer(), x, y, x + width, y + height, color);
}

void Gizmos::DrawSquareRounded(Vec2 origin, float width, std::uint16_t radius) {
    DrawRectangleRounded(origin, width, width, radius);
}

void Gizmos::DrawRectangleRounded(Vec2 origin, float width, float height, std::uint16_t radius) {
    origin = Camera::mainCamera()->WorldToScreenPoint(origin);
    std::uint16_t x, y;
    width *= pixelsPerSize;
    height *= pixelsPerSize;
    x = origin.x - width / 2;
    y = origin.y - height / 2;

    roundedRectangleColor(Application::GetRenderer(), x, y, x + width, y + height, radius, color);
}

void Gizmos::Draw2DWorldSpace(const Vec2& origin, int depth) {
    int i;
    Vec2 dest1, dest2;
    dest1 = dest2 = origin;
    Color lastColor = color;
    color = 0xaa575757;

    // Draw H and V position
    DrawPosition(std::move(origin));

    for (i = 0; i < depth; ++i) {
        dest1 += Vec2::one;
        DrawPosition(std::move(dest1));

        dest2 += Vec2::minusOne;
        DrawPosition(std::move(dest2));
    }

    color = lastColor;
}

void Gizmos::DrawNavMesh(AIPathFinder::NavMesh* navMesh) {
    Vec2 lastPoint;
    Vec2 a, b;
    AIPathFinder::Neuron* p;
    AIPathFinder::Disposition p1, p2;
    Resolution res;

    if (Camera::mainCamera() == nullptr) {
        return;
    }

    res = Application::getResolution();

    color = 0xf6f6f723;

    navMesh->neuron(Camera::mainCamera()->ScreenToWorldPoint(Vec2(0, 0)), p1);
    navMesh->neuron(Camera::mainCamera()->ScreenToWorldPoint(Vec2(res.width, res.height)), p2);
    int x = p1.x, y = p1.y;
    while (x < p2.x) {
        while (y < p2.y) {
            p = navMesh->neuron(x, y);
            lastPoint = navMesh->PointToWorldPosition(x, y);
            color.r = !p || p->locked() ? 255 : 53;
            color.g = p && p->total() ? 100 : 0;

            // Draw Line H
            b = a = lastPoint;
            a.x -= 0.03f;
            b.x += 0.03f;
            DrawLine(std::move(a), std::move(b));

            // Draw Line V
            b = a = lastPoint;
            a.y -= 0.03f;
            b.y += 0.03f;
            DrawLine(std::move(a), std::move(b));
            ++y;
        }
        y = p1.y;
        ++x;
    }
}

void Gizmos::DrawTriangle(Vec2 origin, float base, float height) {
    Vec2 a, b;
    origin.y -= height / 2;
    a = b = origin;
    //  base /= 2.f;
    a.x -= base;
    b.x += base;
    origin.y += height;

    // draw base
    DrawLine(std::move(a), std::move(b));

    // draw left side
    DrawLine(std::move(a), std::move(origin));

    // draw right side
    DrawLine(std::move(b), std::move(origin));
}

void RoninEngine::Runtime::Gizmos::DrawTextOnPosition(Vec2 origin, const std::string& text) {
    Rect r;
    auto cam = Camera::mainCamera();
    // relative to
    origin = cam->WorldToScreenPoint(origin);
    r.x = static_cast<int>(origin.x);
    r.y = static_cast<int>(origin.y);

    UI::Render_String(Application::GetRenderer(), r, text.c_str(), text.length(), 2);
}

void Gizmos::DrawSphere(Vec2 origin, float distance) {
    origin = Camera::mainCamera()->WorldToScreenPoint(origin);
    std::uint16_t x, y, r;
    x = origin.x;
    y = origin.y;
    r = static_cast<std::uint16_t>(distance * pixelsPerSize);
    circleRGBA(Application::GetRenderer(), x, y, r, color.r, color.g, color.b, color.a);
}

void Gizmos::DrawStorm(Vec2 ray, int edges, int delim) {
    /*
    Описание данных stormMember
    Младшие 4 байта, это все для шаги

    stormMember low bits == steps
    stormMember high bits == maxSteps

    stormFlags = int 4 байта (32 бита)
    первые 3 байта (24 бита) = dimensions, от 0 до 0xFFFFFF значений
    остаток 1 байт (8 бит) stormFlags >> 24 = determinants (определители направлений луча)
    0xF000000    xDeterminant = stormFlags >> 24 & 0xF - горизонтальный детерминант x оси (абцис)
    0xF0000000   yDeterminant = stormFlags >> 28       - вертикальный детерминант y оси (ординат)
    */

    Vec2 last = ray;
    std::uint64_t stormMember = 0;
    std::int32_t stormFlags = 1;

    // draw current point

    Color lastColor = Gizmos::color;
    Gizmos::color = 0xfff6f6f7;
    if (edges > 0)
        for (;;) {
            std::uint32_t&& steps = (stormMember & const_storm_steps_flag);
            std::uint32_t&& maxSteps = (stormMember >> 32);
            //Шаг заканчивается (step = turnSteps) происходит поворот
            if (steps % std::max(1u, (maxSteps / 4)) == 0) {
                //переход на новое измерение
                //при steps == maxsteps
                if (steps == maxSteps) {
                    if (--edges <= -1) break;

                    stormMember = (8ul * (stormFlags = stormFlags & const_storm_dimensions)) << 32;
                    stormFlags = ((stormFlags & const_storm_dimensions) + 1) | const_storm_yDeterminant_start;
                } else {
                    if (stormFlags >> 28) {
                        stormFlags ^= stormFlags & const_storm_xDeterminant;                                      // clear x
                        stormFlags |= ((stormFlags & const_storm_yDeterminant) >> 4) & const_storm_xDeterminant;  // x = y
                        stormFlags ^= stormFlags & const_storm_yDeterminant;                                      // clear y
                    } else {
                        stormFlags ^= stormFlags & const_storm_yDeterminant;                                      // clear y
                        stormFlags |= ((stormFlags & const_storm_xDeterminant) << 4) & const_storm_yDeterminant;  // y = x
                        stormFlags ^= const_storm_yDeterminant_inverse;                                           // inverse
                        stormFlags ^= stormFlags & const_storm_xDeterminant;                                      // clear x
                    }
                }
            }

            char&& xDeter = (stormFlags >> 24 & 0xf);
            char&& yDeter = stormFlags >> 28;
            ray.x += xDeter == 2 ? -1 : xDeter;
            ray.y += yDeter == 2 ? -1 : yDeter;

            DrawLine(last / delim, ray / delim);
            last = ray;

            if (!(stormMember & const_storm_steps_flag)) {
                if (yDeter) {
                    stormFlags ^= stormFlags & const_storm_xDeterminant;                                      // clear x
                    stormFlags |= ((stormFlags & const_storm_yDeterminant) >> 4) & const_storm_xDeterminant;  // x = y
                    stormFlags ^= stormFlags & const_storm_yDeterminant;                                      // clear y
                } else if (xDeter) {
                    stormFlags ^= stormFlags & const_storm_yDeterminant;                                      // clear y
                    stormFlags |= ((stormFlags & const_storm_xDeterminant) << 4) & const_storm_yDeterminant;  // y = x
                    stormFlags ^= stormFlags & const_storm_xDeterminant;                                      // clear x
                }
            }

            ++(*reinterpret_cast<std::uint32_t*>(&stormMember));
        }

    Gizmos::color = lastColor;
}

float Gizmos::square_triangle(float base, float height) { return base * height / 2; }
float Gizmos::square(float x) { return x * x; }
float Gizmos::square_rectangle(float a, float b) { return a * b; }
float Gizmos::square_circle(float radius) {
    /*
     *      get the square circle
     *      S = PI * r^2
     *      S = PI * (d/2)^2
     *      S = (PI * d^2) / 4
     *
     */
    return Mathf::PI * Mathf::pow2(radius);
}
float Gizmos::square_mesh(std::list<Vec2>&& vecs) {
    /*
     *    get the square a customize figure
     *    S = square
     *    S1 = cell square
     *    n = closed cell
     *    r = partially closed cell
     *    S = S1 * (n + 1/2 * r)
     *
     */

    float S = -1;

    // TODO: написать алгоритм измерение площади произвольным фигурам
    throw std::runtime_error("algorithm is not implemented");
    return S;
}
}  // namespace RoninEngine::Runtime
