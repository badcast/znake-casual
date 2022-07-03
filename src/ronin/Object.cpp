#include "Object.h"

#include "framework.h"

using namespace RoninEngine;
namespace RoninEngine {
namespace Runtime {

static const char _cloneStr[] = " (clone)";

template <typename T>
T* factory_base(bool initInHierarchy, T* clone, const char* name) {
    if (clone == nullptr) {
        if (name == nullptr)
            GC::gc_push_lvalue<T>(clone);
        else
            GC::gc_push_lvalue<T>(clone, std::string(name));
    } else {
        T* newc;
        if constexpr (std::is_same<T, GameObject>())
            newc = Instantiate(clone);
        else if constexpr (std::is_same<T, Transform>()) {
            // TODO: Required cloning for Transform
            GC::gc_push_lvalue(newc);
        } else if constexpr (std::is_same<T, SpriteRenderer>()) {
            GC::gc_push_lvalue(newc, *clone);
        } else if constexpr (std::is_same<T, Camera2D>()) {
            GC::gc_push_lvalue(newc, *clone);
        } else {
            static_assert(true, "undefined type");
            newc = nullptr;
        }
        clone = newc;
    }

    if (clone == nullptr) throw std::bad_alloc();

    if constexpr (std::is_same<T, GameObject>()) {
        if (initInHierarchy) {
            if (RoninEngine::Level::getScene() == nullptr) throw std::runtime_error("var pCurrentScene is null");

            if (!RoninEngine::Level::getScene()->is_hierarchy())
                throw std::runtime_error("var pCurrentScene->mainObject is null");

            auto mainObj = Level::getScene()->main_object;
            auto root = mainObj->transform();
            Transform* tr = ((GameObject*)clone)->transform();
            root->child_append(tr);
        }
    }

    return clone;
}

Transform* create_empty_transform() { return factory_base<Transform>(false, nullptr, nullptr); }
GameObject* create_empty_gameobject() { return factory_base<GameObject>(false, nullptr, nullptr); }

/*NOTE: WoW: Здесь профиксина 6 месячная проблема
template GameObject* CreateObject<GameObject>();
template Transform* CreateObject<Transform>();
template Player* CreateObject<Player>();
template Camera2D* CreateObject<Camera2D>();
template Spotlight* CreateObject<Spotlight>();
template SpriteRenderer* CreateObject<SpriteRenderer>();

template GameObject* CreateObject<GameObject>(const string&);
template Transform* CreateObject<Transform>(const string&);
template Player* CreateObject<Player>(const string&);
template Camera2D* CreateObject<Camera2D>(const string&);
template Spotlight* CreateObject<Spotlight>(const string&);
template SpriteRenderer* CreateObject<SpriteRenderer>(const string&);
-------------------------------------------------------------------*/

GameObject* CreateGameObject() { return factory_base<GameObject>(true, nullptr, nullptr); }

GameObject* CreateGameObject(const string& name) { return factory_base<GameObject>(true, nullptr, name.data()); }

void Destroy(Object* obj) { Destroy(obj, 0); }

void Destroy(Object* obj, float t) {
    if (!obj || !Level::getScene()) throw std::bad_exception();
    if (!Level::getScene()->_destructions) {
        Level::getScene()->_destructions =
            GC::gc_alloc<std::remove_pointer<decltype(Level::getScene()->_destructions)>::type>();
    }

    auto ref = Level::getScene()->_destructions;

    auto iter = std::find_if(std::begin(*ref), std::end(*ref), [obj](pair<Object*, float> x) { return obj == x.first; });

    if (iter != std::end(*ref)) std::runtime_error("Object is destroyed state");

    ref->push_back(make_pair(const_cast<Object*>(obj), Time::time() + t));
}

void Destroy_Immediate(Object* obj) {
    if (!obj) throw std::runtime_error("Object is null");

    GameObject* gObj;
    if ((gObj = dynamic_cast<GameObject*>(obj)) != nullptr) {
        if (Level::getScene()->_firstRunScripts) {
            Level::getScene()->_firstRunScripts->remove_if([gObj](Behaviour* x) {
                auto iter = find_if(std::begin(gObj->m_components), std::end(gObj->m_components),
                                    [x](Component* c) { return (Component*)x == c; });

                return iter != end(gObj->m_components);
            });
        } else if (Level::getScene()->_realtimeScripts) {
            Level::getScene()->_realtimeScripts->remove_if([gObj](Behaviour* x) {
                auto iter = find_if(std::begin(gObj->m_components), std::end(gObj->m_components),
                                    [x](Component* c) { return (Component*)x == c; });

                return iter != end(gObj->m_components);
            });
        }
        else // destroy other types
        {
            //FIXME: destroy other types from gameobject->m_components (delete a list)
        }
    }

    // todo: деструктор для этого объекта
    SDL_Log("Object destroyed id: %lu", obj->id);

    Level::getScene()->_objects.erase(obj);
    GC::gc_unload(obj);
}

bool instanced(Object* obj) {
    if (!obj || !Level::getScene()) throw std::bad_exception();
    auto iter = Level::getScene()->_objects.find(obj);
    return iter != end(Level::getScene()->_objects);
}

GameObject* Instantiate(GameObject* obj) {
    GameObject* clone = CreateGameObject();
    clone->m_name = obj->m_name;
    if (clone->m_name.find(_cloneStr) == std::string::npos) clone->m_name += _cloneStr;

    for (auto iter = begin(obj->m_components); iter != end(obj->m_components); ++iter) {
        Component* replacement = *iter;
        if (Transform* t = dynamic_cast<Transform*>(replacement)) {
            Transform* existent = clone->transform();
            existent->_angle = t->_angle;
            existent->position(t->position());
            // BUG: Hierarchy cnage is bug, fix now;
            //  Clone childs recursive
            for (Transform* y : t->hierarchy) {
                GameObject* yClone = Instantiate(y->gameObject());
                yClone->transform()->setParent(existent);
                yClone->m_name = t->gameObject()->m_name;  // put " (clone)" name
                yClone->m_name.shrink_to_fit();
            }
            //skip transform existent component
            continue;
        } else if (dynamic_cast<SpriteRenderer*>(replacement)) {
            replacement = factory_base<SpriteRenderer>(false, reinterpret_cast<SpriteRenderer*>(replacement), nullptr);
        } else if (dynamic_cast<Camera2D*>(replacement)) {
            replacement = factory_base<Camera2D>(false, reinterpret_cast<Camera2D*>(replacement), nullptr);
        } else {
            static_assert(true, "Undefined type");
            continue;
        }

        replacement->_derivedObject = nullptr;
        clone->Add_Component(replacement);
    }

    return clone;
}
GameObject* Instantiate(GameObject* obj, Vec2 position, float angle) {
    obj = Instantiate(obj);
    obj->transform()->position(position);
    obj->transform()->angle(angle);
    return obj;
}
GameObject* Instantiate(GameObject* obj, Vec2 position, Transform* parent, bool worldPositionState) {
    throw std::bad_alloc();
    obj = Instantiate(obj);
    obj->transform()->position(position);
    // TODO: arg parent and worldPositionState not implemented. Checkout
    return obj;
}

// base class

Object::Object() : Object(typeid(Object).name()) {}

Object::Object(const string& name) : m_name(name) {
    m_name.shrink_to_fit();
    id = Level::getScene()->globalID++;
    Level::getScene()->ObjectPush(this);
}

void Object::Destroy() { RoninEngine::Runtime::Destroy(this); }

std::string& Object::name(const std::string& newName) { return (m_name = newName); }

const string& Object::name() { return m_name; }

Object::operator bool() { return instanced(this); }
}  // namespace Runtime
}  // namespace RoninEngine
