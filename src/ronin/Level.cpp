#include <set>

#include "framework.h"

namespace RoninEngine {
Level *pCurrentScene;

std::map<float, std::set<Transform *>> matrixWorld;

namespace Runtime {
void callback_movement(Transform *transform, Vec2 lastPoint) {
    Vec2 newPoint(Vec2::Round(transform->position()));
    lastPoint = Vec2::Round(lastPoint);

    // skip at no changed pos
    if (newPoint == lastPoint) return;

    // 1. delete last point source
    matrixWorld[lastPoint.magnitude()].erase(transform);

    // 2. add new point source
    matrixWorld[newPoint.magnitude()].insert(transform);
}
}  // namespace Runtime

Level::Level() : Level("Untitled scene") {}
Level::Level(const string &name) {
    if (pCurrentScene != nullptr) {
        static_assert("pCurrentScene is and replaced by new");
    }
    main_object = nullptr;
    pCurrentScene = this;
    _firstRunScripts = nullptr;
    _realtimeScripts = nullptr;
    _destructions = nullptr;
    globalID = 0;
    m_isUnload = false;
    m_name = name;
    matrixWorld.clear();

    GC::gc_alloc_lval(ui, this);
}
Level::~Level() {
    GameObject *target = main_object;  // first
    Transform *tr;
    list<GameObject *> stack;

    if (pCurrentScene == this) {
        static_assert("pCurrentScene set to null");
        pCurrentScene = nullptr;
    }

    if (_firstRunScripts) {
        GC::gc_unalloc(_firstRunScripts);
    }
    if (_realtimeScripts) {
        GC::gc_unalloc(_realtimeScripts);
    }
    if (_destructions) {
        GC::gc_unalloc(_destructions);
    }
    /*
        // free objects
        while (target) {
            tr = target->transform();
            for (auto c : tr->hierarchy) {
                stack.emplace_back(c->gameObject());
            }

            GC::gc_unload(target);

            if (!stack.empty()) {
                target = stack.front();
                stack.pop_front();
            } else
                target = nullptr;
        }
    */
    this->_objects.clear();

    GC::gc_unalloc(ui);
}

void Level::CC_Render_Push(Renderer *rend) { _assoc_renderers.emplace_front(rend); }

void Level::CC_Light_Push(Light *light) { _assoc_lightings.emplace_front(light); }

void Level::ObjectPush(Object *obj) { _objects.insert(make_pair(obj, Time::time())); }

std::vector<RoninEngine::Runtime::Transform *> *RoninEngine::Level::getInternalTransforms(Runtime::Transform *parent) {
    if (!parent) {
        Application::fail("Argument is null");
    }
    return &parent->hierarchy;
}

void Level::PinScript(Behaviour *behav) {
    if (!_firstRunScripts) GC::gc_alloc_lval(_firstRunScripts);
    if (std::find_if(begin(*this->_firstRunScripts), end(*this->_firstRunScripts),
                     [behav](Behaviour *ref) { return behav == ref; }) == end(*_firstRunScripts)) {
        if (_realtimeScripts && std::find_if(begin(*this->_realtimeScripts), end(*this->_realtimeScripts),
                                             [behav](Behaviour *ref) { return behav == ref; }) != end(*_realtimeScripts))
            return;

        _firstRunScripts->emplace_back(behav);
    }
}

void Level::RenderLevel(SDL_Renderer *renderer) {
    if (_firstRunScripts) {
        if (!_realtimeScripts) {
            GC::gc_alloc_lval(_realtimeScripts);
            if (_realtimeScripts == nullptr) throw std::bad_alloc();
        }

        for (auto x : *_firstRunScripts) {
            x->OnStart();  // go start (first draw)
            this->_realtimeScripts->emplace_back(x);
        }
        GC::gc_unalloc(_firstRunScripts);
        _firstRunScripts = nullptr;
    }

    if (_realtimeScripts) {
        for (auto n : *_realtimeScripts) {
            n->OnUpdate();
        };
    }

    // Render on main camera
    Camera *cam = Camera::mainCamera();  // Draw level
    if (cam) {
        Resolution res = Application::getResolution();
        // FlushCache last result
        if (cam->targetClear) cam->__rendererOutResults.clear();
        // Рисуем в соотношение окна...
        cam->render(renderer, {0, 0, res.width, res.height}, main_object);
    }

    // Destroy req objects
    if (_destructions) {
        if (_destructions->begin() == _destructions->end()) {
            GC::gc_unalloc(_destructions);
        } else {
            _destructions->remove_if([](pair<Object *, float> &p) {
                if (p.second <= Time::time()) {
                    Destroy_Immediate(p.first);
                    return true;
                }
                return false;
            });
        }
    }
}

void Level::RenderUI(SDL_Renderer *renderer) {
    // render UI
    ui->Do_Present(renderer);
}

void Level::RenderSceneLate(SDL_Renderer *renderer) {
    if (_realtimeScripts) {
        for (auto n : *_realtimeScripts) {
            n->OnLateUpdate();
        }
    }
}
bool Level::is_hierarchy() { return this->main_object != nullptr; }
std::string &Level::name() { return this->name(); }
UI::GUI *Level::Get_GUI() { return this->ui; }
void Level::Unload() { this->m_isUnload = true; }
Level *Level::getScene() { return pCurrentScene; }
}  // namespace RoninEngine
