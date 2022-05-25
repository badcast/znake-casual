#ifndef ___framework_H
#define ___framework_H

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
#include <memory>
#include <new>
#include <stdexcept>
#include <string>
#include <tuple>
#include <typeinfo>
#include <vector>

// libx/dev/modules/
#include "../../../libx/dev/modules/config_fastfile.h"
#include "../../../libx/dev/modules/queue_list.h"

#include "Color.h"
#include "Controls.h"
#include "Font.h"
#include "Paths.h"
#include "Texture.h"
#include "Time.h"
#include "Types.h"
#include "global_val.h"

namespace RoninEngine {
class Application;
class Level;

// Pre declaration
namespace Runtime {
class GC;
class Object;
class Component;
class Renderer;
class SpriteRenderer;
class Transform;
class Camera;
class Camera2D;
class GameObject;
class Texture;
class Atlas;
class Sprite;
class Light;
class Behaviour;
struct Color;
}  // namespace Runtime
}  // namespace RoninEngine

#endif
