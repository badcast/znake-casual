#include "Component.h"

#include "GameObject.h"
#include "framework.h"

namespace RoninEngine {
namespace Runtime {
// base component
Component::Component() : Component(typeid(Component).name()) {}

Component::Component(const string& name)
    : Object(name), _derivedObject(nullptr) {}

const bool Component::isBind() { return _derivedObject != nullptr; }

GameObject* Component::gameObject() { return _derivedObject; }

Transform* Component::transform() {
    if (!isBind()) throw std::runtime_error("This component isn't binding");
    return _derivedObject->getComponent<Transform>();
}
}  // namespace Runtime
}  // namespace RoninEngine
