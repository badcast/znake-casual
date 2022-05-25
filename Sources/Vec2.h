#pragma once


#include "framework.h"

namespace RoninEngine {

// TODO: Требует принудительной оптимизации.

struct Vec2 {
   public:
    float x;
    float y;

    Vec2();
    Vec2(float x, float y);
    //extern Vec2(float && x, float && y);
    Vec2(const Vec2&) = default;
    ~Vec2() = default;

    static const Vec2 one;
    static const Vec2 half;
    static const Vec2 minusOne;
    static const Vec2 zero;
    static const Vec2 down;
    static const Vec2 left;
    static const Vec2 right;
    static const Vec2 up;
    static const Vec2 infinity;
    static const Vec2 negativeInfinity;

   public:
    float magnitude() const;
    float sqrMagnitude() const;
    Vec2 normalized();
    void Normalize();

    static const Vec2 Abs(const Vec2& value);
    static const Vec2 NAbs(const Vec2& value);
    static Vec2 Slerp(const Vec2& a, const Vec2& b, float t);
    static Vec2 SlerpUnclamped(const Vec2& a, const Vec2& b, float t);
    static Vec2 Lerp(const Vec2& a, const Vec2& b, float t);
    static Vec2 LerpUnclamped(const Vec2& a, const Vec2& b, float t);
    static Vec2 Max(const Vec2& lhs, const Vec2& rhs);
    static Vec2 Min(const Vec2& lhs, const Vec2& rhs);
    static Vec2 MoveTowards(const Vec2& current, const Vec2& target,
                            float maxDistanceDelta);
    static Vec2 Reflect(const Vec2& inDirection, const Vec2& inNormal);
    static Vec2 Scale(const Vec2& a, const Vec2& b);
    static float Distance(const Vec2& lhs, const Vec2& rhs);
    static float Angle(const Vec2& from, const Vec2& to);
    static float SignedAngle(const Vec2& from, const Vec2& to);
    static Vec2 ClampMagnitude(Vec2 vector, float maxLength);
    static Vec2 SmoothDamp(Vec2 current, Vec2 target, Vec2& currentVelocity,
                           float smoothTime, float maxSpeed, float deltaTime);
    static float Dot(const Vec2& lhs, const Vec2& rhs);
    static float SqrMagnitude(const Vec2& lhs);
    static bool AreaHasIntersection(const SDL_Rect* A, const SDL_Rect* B);
    static bool AreaIntersectRect(const SDL_FRect& A, const SDL_FRect& B,
                                  SDL_FRect& result);
    static void Area_UnionRect(const SDL_Rect* A, const SDL_Rect* B,
                               SDL_Rect* result);
    //	static bool AreaEnclosePoints(const Rectf_t* points, int count, const
    //Rectf_t* clip, Rectf_t* result);
    static bool AreaPointInRect(const Vec2& p, const SDL_FRect& r);
    static bool InArea(const Vec2& p, const SDL_FRect& r);
    static const Vec2 Rotate(float angle, Vec2 v);
    static const Vec2 RotateUp(float angle, Vec2 v);

    Vec2& operator+=(const Vec2& rhs);
    Vec2& operator-=(const Vec2& rhs);
    Vec2& operator*=(const float& d);
    Vec2& operator/=(const float& d);
    Vec2& operator=(const Vec2& rhs);
};

Vec2 operator+(const Vec2& lhs, const Vec2& rhs);
Vec2 operator-(const Vec2& lhs, const Vec2& rhs);
bool operator==(const Vec2& lhs, const Vec2& rhs);
bool operator!=(const Vec2& lhs, const Vec2& rhs);
Vec2 operator*(const float& d, const Vec2& rhs);
Vec2 operator/(const float& d, const Vec2& rhs);
Vec2 operator*(const Vec2& rhs, const float& d);
Vec2 operator/(const Vec2& rhs, const float& d);

}  // namespace RoninEngine
