#pragma once

#include "Object.h"
#include "Transform.h"
#include "dependency.h"

namespace RoninEngine {
namespace Runtime {
//[attrib class]
template <typename T>
class AttribGetTypeHelper {
   public:
    static T* getType(std::list<Component*>& container) {
        auto iter = find_if(++begin(container), end(container), [](Component* c) { return dynamic_cast<T*>(c) != nullptr; });

        if (iter != end(container)) return reinterpret_cast<T*>(*iter);

        return nullptr;
    }
};

class GameObject final : public Object {
    friend Camera2D;
    friend Component;
    friend GameObject* Instantiate(GameObject* obj);
    friend GameObject* Instantiate(GameObject* obj, Vec2 position, float angle);
    friend GameObject* Instantiate(GameObject* obj, Vec2 position, Transform* parent, bool worldPositionState);
    friend void Destroy(Object* obj);
    friend void Destroy(Object* obj, float t);
    friend void Destroy_Immediate(Object* obj);

   private:
    std::list<Component*> m_components;

   public:
    GameObject();

    GameObject(const std::string&);

    GameObject(const GameObject&) = delete;

    ~GameObject();

    Transform* transform();

    Component* addComponent(Component* component);

    SpriteRenderer* spriteRenderer() { return getComponent<SpriteRenderer>(); }

    Camera2D* camera2D() { return getComponent<Camera2D>(); }

    Terrain2D* terraind2D() { return getComponent<Terrain2D>();}

    template <typename T>
    std::enable_if_t<std::is_base_of<Component, T>::value, T*> addComponent();

    template <typename T>
    T* getComponent() {
        return AttribGetTypeHelper<T>::getType(this->m_components);
    }
};

}  // namespace Runtime
}  // namespace RoninEngine
