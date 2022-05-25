#ifndef _TYPES_CH
#define _TYPES_CH

#include "Vec2.h"

namespace RoninEngine {

struct Vec2;
struct point_t;

enum Align { Left, Right, Center };

class point_t {
   public:
    int x;
    int y;
    point_t() : x(0), y(0) {}
    point_t(int _x, int _y) : x(_x), y(_y) {}

    point_t operator+(const point_t& rhs) { return {this->x + rhs.x, this->y + rhs.y}; }

    point_t operator-(const point_t& rhs) { return {this->x - rhs.x, this->y + rhs.y}; }

    point_t operator*(const int& rhs) { return {this->x * rhs, this->y * rhs}; }

    point_t operator/(const int& rhs) { return {this->x / rhs, this->y / rhs}; }

    point_t& operator+=(const point_t& rhs) {
        this->x += rhs.x;
        this->y += rhs.y;
        return *this;
    }

    point_t& operator-=(const point_t& rhs) {
        this->x -= rhs.x;
        this->y -= rhs.y;
        return *this;
    }

    point_t& operator*=(const int& rhs) {
        this->x *= rhs;
        this->y *= rhs;
        return *this;
    }

    point_t& operator/=(const int& rhs) {
        this->x /= rhs;
        this->y /= rhs;
        return *this;
    }
};

template <typename T>
class xRect {
   public:
    T x, y, w, h;

    xRect() { x = y = w = h = 0; }
    xRect(const T& x, const T& y, const T& w, const T& h) {
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
    }

    bool empty() const { return x != 0 && y != 0 && w != 0 && h != 0; }

    xRect<T> operator+(const xRect<T>& rhs) { return {this->x + rhs.x, this->y + rhs.y, this->w + rhs.w, this->h + rhs.h}; }

    xRect<T> operator-(const xRect<T>& rhs) { return {this->x - rhs.x, this->y + rhs.y, this->w - rhs.w, this->h - rhs.h}; }

    xRect<T> operator*(const T& rhs) { return {this->x * rhs, this->y * rhs, this->w * rhs, this->h * rhs}; }

    xRect<T> operator/(const T& rhs) { return {this->x / rhs, this->y / rhs, this->w / rhs, this->h / rhs}; }

    xRect<T>& operator+=(const xRect<T>& rhs) {
        this->x += rhs.x;
        this->y += rhs.y;
        this->w += rhs.w;
        this->h += rhs.h;
        return *this;
    }

    xRect<T>& operator-=(const xRect<T>& rhs) {
        this->x -= rhs.x;
        this->y -= rhs.y;
        this->w -= rhs.w;
        this->h -= rhs.h;
        return *this;
    }

    xRect<T>& operator*=(const T& rhs) {
        this->x *= rhs;
        this->y *= rhs;
        return *this;
    }

    xRect<T>& operator/=(const T& rhs) {
        this->x /= rhs;
        this->y /= rhs;
        return *this;
    }

    static const xRect<T> zero;
    static const xRect<T> one;
};

typedef xRect<int> Rect_t;
typedef xRect<float> Rectf_t;

double Get_Angle(Vec2 lhs, Vec2 rhs);
}  // namespace RoninEngine
#endif
