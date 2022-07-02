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
        decltype(end(container)) iter =
            find_if(begin(container), end(container), [](Component* c) { return dynamic_cast<T*>(c) != 0; });

        if (iter != end(container)) return dynamic_cast<T*>(*iter);

        return nullptr;
    }

    static T* getComponent(GameObject* obj);
};

class GameObject final : public Object {
    friend Camera2D;
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
    // GameObject(const GameObject&) = delete;
    virtual ~GameObject();

    Transform* transform();

    Component* Add_Component(Component* component);

    template <typename T>
    std::enable_if_t<std::is_base_of<Component, T>::value, T*> Add_Component();

    template <typename T>
    T* Get_Component() {
        return AttribGetTypeHelper<T>::getType(this->m_components);
    }
};
}  // namespace Runtime
}  // namespace RoninEngine
