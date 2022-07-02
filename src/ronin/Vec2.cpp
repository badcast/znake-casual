#include "Vec2.h"

#include "framework.h"

using namespace RoninEngine;

const Vec2 Vec2::one = Vec2(1, 1);
const Vec2 Vec2::half = one / 2;
const Vec2 Vec2::minusOne = Vec2::one * -1;
const Vec2 Vec2::zero = Vec2(0, 0);
const Vec2 Vec2::down = Vec2(0, -1);
const Vec2 Vec2::left = Vec2(-1, 0);
const Vec2 Vec2::right = Vec2(1, 0);
const Vec2 Vec2::up = Vec2(0, 1);
const Vec2 Vec2::infinity = Vec2(Mathf::Infinity, Mathf::Infinity);
const Vec2 Vec2::negativeInfinity = Vec2(Mathf::Infinity, Mathf::Infinity) * -1;

Vec2::Vec2() : x(0), y(0) {}

Vec2::Vec2(float x, float y) {
    this->x = x;
    this->y = y;
}

float Vec2::magnitude() const { return Mathf::sqrt(x * x + y * y); }

float Vec2::sqrMagnitude() const { return x * x + y * y; }

Vec2 Vec2::normalized() {
    Vec2 result = *this;
    result.Normalize();
    return result;
}

void Vec2::Normalize() {
    float mag = magnitude();
    if (mag > 1E-05) {
        *this /= mag;
    } else {
        *this = Vec2::zero;
    }
}

const Vec2 Vec2::Abs(const Vec2& value) { return {Mathf::abs(value.x), Mathf::abs(value.y)}; }
const Vec2 Vec2::NAbs(const Vec2& value) { return {Mathf::nabs(value.x), Mathf::nabs(value.y)}; }

Vec2 Vec2::Slerp(const Vec2& a, const Vec2& b, float t) {
    t = Mathf::Clamp01(t);

    // get cosine of angle between disposition (-1 -> 1)
    float CosAlpha = Dot(a, b);
    // get angle (0 -> pi)
    float Alpha = Mathf::acos(CosAlpha);
    // get sine of angle between disposition (0 -> 1)
    float SinAlpha = Mathf::sin(Alpha);
    // this breaks down when SinAlpha = 0, i.e. Alpha = 0 or pi
    float t1 = Mathf::sin((static_cast<float>(1) - t) * Alpha) / SinAlpha;
    float t2 = Mathf::sin(t * Alpha) / SinAlpha;

    // interpolate src disposition
    return a * t1 + b * t2;
}
Vec2 Vec2::SlerpUnclamped(const Vec2& a, const Vec2& b, float t) {
    // get cosine of angle between disposition (-1 -> 1)
    float CosAlpha = Dot(a, b);
    // get angle (0 -> pi)
    float Alpha = Mathf::acos(CosAlpha);
    // get sine of angle between disposition (0 -> 1)
    float SinAlpha = Mathf::sin(Alpha);
    // this breaks down when SinAlpha = 0, i.e. Alpha = 0 or pi
    float t1 = Mathf::sin((static_cast<float>(1) - t) * Alpha) / SinAlpha;
    float t2 = Mathf::sin(t * Alpha) / SinAlpha;

    // interpolate src disposition
    return a * t1 + b * t2;
}
Vec2 Vec2::Lerp(const Vec2& a, const Vec2& b, float t) {
    t = Mathf::Clamp01(t);
    return Vec2(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t);
}

Vec2 Vec2::LerpUnclamped(const Vec2& a, const Vec2& b, float t) { return Vec2(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t); }

Vec2 Vec2::Max(const Vec2& lhs, const Vec2& rhs) { return Vec2(Mathf::Max(lhs.x, rhs.x), Mathf::Max(lhs.y, rhs.y)); }

Vec2 Vec2::Min(const Vec2& lhs, const Vec2& rhs) { return Vec2(Mathf::Min(lhs.x, rhs.x), Mathf::Min(lhs.y, rhs.y)); }

Vec2 Vec2::MoveTowards(const Vec2& current, const Vec2& target, float maxDistanceDelta) {
    Vec2 a = target - current;
    float mag = a.magnitude();
    Vec2 result;
    if (mag <= maxDistanceDelta || mag == 0) {
        result = target;
    } else {
        result = current + a / mag * maxDistanceDelta;
    }
    return result;
}

Vec2 Vec2::Reflect(const Vec2& inDirection, const Vec2& inNormal) {
    return -2 * Dot(inNormal, inDirection) * inNormal + inDirection;
}

Vec2 Vec2::Scale(const Vec2& a, const Vec2& b) { return Vec2(a.x * b.x, a.y * b.y); }

float Vec2::Distance(const Vec2& lhs, const Vec2& rhs) { return (lhs - rhs).magnitude(); }

float Vec2::Angle(const Vec2& from, const Vec2& to) {
    auto n1 = from;
    auto n2 = to;
    n1.Normalize();
    n2.Normalize();
    return Mathf::acos(Mathf::Clamp(Vec2::Dot(n1, n2), -1, 1));
}

float Vec2::SignedAngle(const Vec2& from, const Vec2& to) {
    Vec2 norm = from;
    norm.Normalize();
    Vec2 norm2 = to;
    norm2.Normalize();
    float num = Angle(from, to);
    float num2 = Mathf::sign(norm.x * norm2.y - norm.y * norm2.x);

    return num * num2;
}

Vec2 Vec2::ClampMagnitude(Vec2 vector, float maxLength) {
    Vec2 result;
    if (vector.sqrMagnitude() > maxLength * maxLength) {
        result = vector.normalized() * maxLength;
    } else {
        result = vector;
    }
    return result;
}

Vec2 Vec2::SmoothDamp(Vec2 current, Vec2 target, Vec2& currentVelocity, float smoothTime, float maxSpeed, float deltaTime) {
    smoothTime = Mathf::Max(0.f, Mathf::Max(0001.f, smoothTime));
    float num = 2 / smoothTime;
    float num2 = num * deltaTime;
    float d = 1 / (1 + num2 + 0, 48 * num2 * num2 + 0, 235 * num2 * num2 * num2);
    Vec2 vector = current - target;
    Vec2 vector2 = target;
    float maxLength = maxSpeed * smoothTime;
    vector = ClampMagnitude(vector, maxLength);
    target = current - vector;
    Vec2 vector3 = (currentVelocity + num * vector) * deltaTime;
    currentVelocity = (currentVelocity - num * vector3) * d;
    Vec2 vector4 = target + (vector + vector3) * d;
    if (Vec2::Dot(vector2 - current, vector4 - vector2) > 0) {
        vector4 = vector2;
        currentVelocity = (vector4 - vector2) / deltaTime;
    }
    return vector4;
}

float Vec2::Dot(const Vec2& lhs, const Vec2& rhs) { return lhs.x * rhs.x + lhs.y * rhs.y; }

float Vec2::SqrMagnitude(const Vec2& lhs) { return lhs.x * lhs.x + lhs.y * lhs.y; }

bool Vec2::AreaHasIntersection(const SDL_Rect* A, const SDL_Rect* B) {
    int Amin, Amax, Bmin, Bmax;

    if (!A) {
        SDL_InvalidParamError("A");
        return false;
    }

    if (!B) {
        SDL_InvalidParamError("B");
        return false;
    }

    /* Special cases for empty rects */
    if (SDL_RectEmpty(A) || SDL_RectEmpty(B)) {
        return false;
    }

    /* Horizontal intersection */
    Amin = A->x;
    Amax = Amin + A->w;
    Bmin = B->x;
    Bmax = Bmin + B->w;
    if (Bmin > Amin) Amin = Bmin;
    if (Bmax < Amax) Amax = Bmax;
    if (Amax <= Amin) return false;

    /* Vertical intersection */
    Amin = A->y;
    Amax = Amin + A->h;
    Bmin = B->y;
    Bmax = Bmin + B->h;
    if (Bmin > Amin) Amin = Bmin;
    if (Bmax < Amax) Amax = Bmax;
    if (Amax <= Amin) return false;

    return true;
}

bool Vec2::AreaIntersectRect(const SDL_FRect& A, const SDL_FRect& B, SDL_FRect& result) {
    float Amin, Amax, Bmin, Bmax;

    /* Special cases for empty rects */
    if (SDL_FRectEmpty(&A) || SDL_FRectEmpty(&B)) {
        result.h = 0;
        result.h = 0;
        return false;
    }

    /* Horizontal intersection */
    Amin = A.x;
    Amax = Amin + A.w;
    Bmin = B.x;
    Bmax = Bmin + B.w;
    if (Bmin > Amin) Amin = Bmin;
    result.x = Amin;
    if (Bmax < Amax) Amax = Bmax;
    result.w = Amax - Amin;

    /* Vertical intersection */
    Amin = A.y;
    Amax = Amin + A.h;
    Bmin = B.y;
    Bmax = Bmin + B.h;
    if (Bmin > Amin) Amin = Bmin;
    result.y = Amin;
    if (Bmax < Amax) Amax = Bmax;
    result.h = Amax - Amin;

    return !SDL_FRectEmpty(&result);
}

void Vec2::Area_UnionRect(const SDL_Rect* A, const SDL_Rect* B, SDL_Rect* result) {
    int Amin, Amax, Bmin, Bmax;

    if (!A) {
        SDL_InvalidParamError("A");
        return;
    }

    if (!B) {
        SDL_InvalidParamError("B");
        return;
    }

    if (!result) {
        SDL_InvalidParamError("result");
        return;
    }

    /* Special cases for empty Rects */
    if (SDL_RectEmpty(A)) {
        if (SDL_RectEmpty(B)) {
            /* A and B empty */
            return;
        } else {
            /* A empty, B not empty */
            *result = *B;
            return;
        }
    } else {
        if (SDL_RectEmpty(B)) {
            /* A not empty, B empty */
            *result = *A;
            return;
        }
    }

    /* Horizontal union */
    Amin = A->x;
    Amax = Amin + A->w;
    Bmin = B->x;
    Bmax = Bmin + B->w;
    if (Bmin < Amin) Amin = Bmin;
    result->x = Amin;
    if (Bmax > Amax) Amax = Bmax;
    result->w = Amax - Amin;

    /* Vertical union */
    Amin = A->y;
    Amax = Amin + A->h;
    Bmin = B->y;
    Bmax = Bmin + B->h;
    if (Bmin < Amin) Amin = Bmin;
    result->y = Amin;
    if (Bmax > Amax) Amax = Bmax;
    result->h = Amax - Amin;
}

Vec2 Vec2::Round(const Vec2& lhs) { return {Mathf::round(lhs.x), Mathf::round(lhs.y)}; }

bool AreaEnclosePoints(const Rectf_t* points, int count, const Rectf_t* clip, Rectf_t* result) {
    float minx = 0;
    float miny = 0;
    float maxx = 0;
    float maxy = 0;
    float x, y;
    int i;

    if (count < 1) {
        SDL_InvalidParamError("count");
        return false;
    }

    if (clip) {
        bool added = false;
        const int clip_minx = clip->x;
        const int clip_miny = clip->y;
        const int clip_maxx = clip->x + clip->w - 1;
        const int clip_maxy = clip->y + clip->h - 1;

        /* Special case for empty rectangle */
        if (clip->empty()) {
            return false;
        }

        for (i = 0; i < count; ++i) {
            x = points[i].x;
            y = points[i].y;

            if (x < clip_minx || x > clip_maxx || y < clip_miny || y > clip_maxy) {
                continue;
            }
            if (!added) {
                /* Special case: if no result was requested, we are done */
                if (result == nullptr) {
                    return true;
                }

                /* First point added */
                minx = maxx = x;
                miny = maxy = y;
                added = true;
                continue;
            }
            if (x < minx) {
                minx = x;
            } else if (x > maxx) {
                maxx = x;
            }
            if (y < miny) {
                miny = y;
            } else if (y > maxy) {
                maxy = y;
            }
        }
        if (!added) {
            return false;
        }
    } else {
        /* Special case: if no result was requested, we are done */
        if (result == nullptr) {
            return true;
        }

        /* No clipping, always add the first point */
        minx = maxx = points[0].x;
        miny = maxy = points[0].y;

        for (i = 1; i < count; ++i) {
            x = points[i].x;
            y = points[i].y;

            if (x < minx) {
                minx = x;
            } else if (x > maxx) {
                maxx = x;
            }
            if (y < miny) {
                miny = y;
            } else if (y > maxy) {
                maxy = y;
            }
        }
    }

    if (result) {
        result->x = minx;
        result->y = miny;
        result->w = (maxx - minx) + 1;
        result->h = (maxy - miny) + 1;
    }
    return true;
}

bool Vec2::AreaPointInRect(const Vec2& p, const SDL_FRect& r) {
    return ((p.x >= r.x) && (p.x < (r.x + r.w)) && (p.y >= r.y) && (p.y < (r.y + r.h))) ? true : false;
}

bool Vec2::InArea(const Vec2& p, const SDL_FRect& r) { return p.x >= r.x && p.x <= r.w && p.y >= r.h && p.y <= r.y; }

const Vec2 Vec2::Rotate(float angle, Vec2 v) {
    float Cos = Mathf::cos(angle);
    float Sin = Mathf::sin(angle);
    v.x = v.x * Cos - v.y * Sin;
    v.y = v.x * Sin + v.y * Cos;
    return v;
}

const Vec2 Vec2::RotateUp(float angle, Vec2 v) {
    float Cos = Mathf::cos(angle);
    float Sin = Mathf::sin(angle);
    v.x = v.x * Cos + v.y * Sin;
    v.y = -v.x * Sin + v.y * Cos;
    return v;
}

Vec2& Vec2::operator+=(const Vec2& rhs) {
    this->x += rhs.x;
    this->y += rhs.y;
    return *this;
}
Vec2& Vec2::operator-=(const Vec2& rhs) {
    this->x -= rhs.x;
    this->y -= rhs.y;
    return *this;
}

Vec2& Vec2::operator*=(const float& d) {
    this->x *= d;
    this->y *= d;
    return *this;
}

Vec2& Vec2::operator/=(const float& d) {
    this->x /= d;
    this->y /= d;
    return *this;
}

Vec2& Vec2::operator=(const Vec2& rhs) {
    this->x = rhs.x;
    this->y = rhs.y;
    return *this;
}

Vec2 RoninEngine::Runtime::operator+(const Vec2& lhs, const Vec2& rhs) { return Vec2(lhs.x + rhs.x, lhs.y + rhs.y); }
Vec2 RoninEngine::Runtime::operator-(const Vec2& lhs, const Vec2& rhs) { return Vec2(lhs.x - rhs.x, lhs.y - rhs.y); }

bool RoninEngine::Runtime::operator==(const Vec2& lhs, const Vec2& rhs) { return (lhs - rhs).sqrMagnitude() < 9.999999E-11; }

bool RoninEngine::Runtime::operator!=(const Vec2& lhs, const Vec2& rhs) { return !(operator==(lhs, rhs)); }

Vec2 RoninEngine::Runtime::operator*(const float& d, const Vec2& rhs) { return Vec2(rhs.x * d, rhs.y * d); }

Vec2 RoninEngine::Runtime::operator/(const float& d, const Vec2& rhs) { return Vec2(rhs.x / d, rhs.y / d); }

Vec2 RoninEngine::Runtime::operator*(const Vec2& rhs, const float& d) { return d * rhs; }

Vec2 RoninEngine::Runtime::operator/(const Vec2& rhs, const float& d) { return d / rhs; }
