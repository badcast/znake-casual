#include "GameObject.h"

#include "Player.h"
#include "pch.h"

using namespace RoninEngine::Runtime;

template Player* GameObject::Add_Component<Player>();
template SpriteRenderer* GameObject::Add_Component<SpriteRenderer>();
template Camera2D* GameObject::Add_Component<Camera2D>();
template Spotlight* GameObject::Add_Component<Spotlight>();

GameObject::GameObject() : GameObject("GameObject") {}

GameObject::GameObject(const string& name) : Object(name) {
    m_components.push_back(create_empty_transform());
    m_components.front()->_derivedObject = this;
}

GameObject::~GameObject() {
    return;
    for (auto x : m_components) {
        GC::gc_unload(x);
    }
}

Transform* GameObject::transform() {
    Transform* t = nullptr;
    for (Component* x : m_components)
        if ((t = dynamic_cast<decltype(t)>(x)) != nullptr) break;

    return t;
}

template <typename T>
T* AttribGetTypeHelper<T>::getComponent(GameObject* hier) {
    return nullptr;
}

Component* GameObject::Add_Component(Component* component) {
    if (!component) throw std::exception();

    if (end(m_components) ==
        std::find_if(begin(m_components), end(m_components), [component](Component* ref) { return component == ref; })) {
        this->m_components.emplace_back(component);

        if (component->_derivedObject) throw bad_exception();

        component->_derivedObject = this;

        if (Behaviour* behav = dynamic_cast<Behaviour*>(component)) {
            Level::getScene()->PinScript(behav);
            behav->OnAwake();
        } else if (Renderer* rend = dynamic_cast<Renderer*>(component)) {
            Level::getScene()->CC_Render_Push(rend);
        } else if (Light* light = dynamic_cast<Light*>(component)) {
            Level::getScene()->CC_Light_Push(light);
        }
    }

    return component;
}

template <typename T>
std::enable_if_t<std::is_base_of<Component, T>::value, T*> GameObject::Add_Component() {
    T* comp = GC::gc_push<T>();
    Add_Component(reinterpret_cast<Component*>(comp));
    return comp;
}
