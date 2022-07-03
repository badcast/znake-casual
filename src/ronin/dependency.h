#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <ctime>
#include <list>
#include <map>
#include <set>
#include <memory>
#include <new>
#include <numeric>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <typeinfo>
#include <vector>

#include "Static.h"
#include "Vec2.h"

namespace RoninEngine {
class Application;
class Level;

namespace UI {
struct RenderData;
class GUI;
}  // namespace UI

namespace AIPathFinder {
class Neuron;
class NavMesh;
}  // namespace AIPathFinder

// Pre declaration
namespace Runtime {

struct Vec2;

class point_t;

struct Render_info;

template <typename T>
class xRect;

typedef xRect<int> Rect_t;
typedef xRect<float> Rectf_t;

class GC;

// base
class Object;
class Component;

// factory
class Transform;
class Camera;
class Camera2D;
class Texture;
class Atlas;
class Sprite;
class Behaviour;
class Terrain2D;
class SpriteRenderer;
class Spotlight;
class GameObject;
class Light;
class Renderer;

struct Color;

// serialize
enum FolderKind {
    // graphics
    GFX,
    // SOUND
    SOUND,
    // MUSIC
    MUSIC,
    // LOADER
    LOADER,
    // LEVELS
    LEVELS,
    // TEXTURES
    TEXTURES
};

}  // namespace Runtime
}  // namespace RoninEngine

#include "Types.h"
#include "Vec2.h"
