#pragma once

#include "dependency.h"

namespace RoninEngine::Runtime {
struct Color {
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
    std::uint8_t a;

    Color() { memset(this, 0, sizeof(Color)); }

    explicit Color(const std::int32_t rgba) { memcpy(this, &rgba, sizeof(Color)); }

    explicit Color(const std::uint32_t rgba) { memcpy(this, &rgba, sizeof(Color)); }

    explicit Color(const char* colorHex) {}

    explicit Color(const std::string& colorHex) : Color(colorHex.c_str()){}

    explicit Color(const std::uint8_t r, const std::uint8_t g, const std::uint8_t b, const std::uint8_t a) {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }

    operator int() { return *reinterpret_cast<int*>(this); }
    operator std::uint32_t() { return *reinterpret_cast<std::uint32_t*>(this); }
};

}  // namespace RoninEngine::Runtime
