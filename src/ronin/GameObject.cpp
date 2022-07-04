#include "framework.h"

using namespace RoninEngine;
using namespace RoninEngine::Runtime;

using Level = RoninEngine::Level;

template Player* GameObject::addComponent<Player>();
template SpriteRenderer* GameObject::addComponent<SpriteRenderer>();
template Camera2D* GameObject::addComponent<Camera2D>();
template Spotlight* GameObject::addComponent<Spotlight>();

GameObject::GameObject() : GameObject("GameObject") {}

GameObject::GameObject(const string& name) : Object(name) {
    m_components.push_back(create_empty_transform());
    m_components.front()->_derivedObject = this;
    Level::self()->_gameObjects.emplace_back(this);
}

GameObject::~GameObject() {
    return;
    for (auto x : m_components) {
        GC::gc_unload(x);
    }
}

inline Transform* GameObject::transform() {
    // NOTE: transform всегда первый объект из контейнера m_components
    return reinterpret_cast<Transform*>(m_components.front());
}

Component* GameObject::addComponent(Component* component) {
    if (!component) throw std::exception();

    if (end(m_components) ==
        std::find_if(begin(m_components), end(m_components), [component](Component* ref) { return component == ref; })) {
        this->m_components.emplace_back(component);

        if (component->_derivedObject) throw bad_exception();

        component->_derivedObject = this;

        if (Behaviour* behav = dynamic_cast<Behaviour*>(component)) {
            Level::self()->PinScript(behav);
            behav->OnAwake();
        } else if (Renderer* rend = dynamic_cast<Renderer*>(component)) {
            Level::self()->CC_Render_Push(rend);
        } else if (Light* light = dynamic_cast<Light*>(component)) {
            Level::self()->CC_Light_Push(light);
        }
    }

    return component;
}

template <typename T>
std::enable_if_t<std::is_base_of<Component, T>::value, T*> GameObject::addComponent() {
    T* comp = GC::gc_push<T>();
    addComponent(reinterpret_cast<Component*>(comp));
    return comp;
}

template <>
Transform* GameObject::getComponent<Transform>() {
    return transform();
}
