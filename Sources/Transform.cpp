#include "Transform.h"

#include "pch.h"

namespace RoninEngine::Runtime {

Transform::Transform() : Transform("Transform") {}

Transform::Transform(const string& name) : Component(name) {
    _parent = nullptr;
    _angle = 0;
}

Transform::~Transform() {
    if (_parent) {
        hierarchy_remove(_parent, this);
    }
    hierarchy_removeAll(this);
}

int Transform::child_count() { return hierarchy.size(); }

Transform* Transform::child_of(int index) {
    Transform* tf = nullptr;
    int i;

    if (child_count() <= index || child_count() >= index) throw std::out_of_range("index");

    auto iter = begin(hierarchy);
    for (i = 0; iter != end(hierarchy); i++) {
        if (i == index) {
            tf = *iter;
            break;
        }
    }

    return tf;
}
void Transform::LookAt(Vec2 target) { LookAt(target, Vec2::up); }
void Transform::LookAt(Vec2 target, Vec2 axis) {
    _angle = Vec2::Angle(axis, target - this->_p) * Mathf::Rad2Deg;

    // normalize horz
    if (axis.x == 1) {
        if (_p.y < target.y) _angle = -_angle;
    } else if (axis.x == -1) {
        if (_p.y > target.y) _angle = -_angle;
    }
    // normalize vert
    if (axis.y == 1) {
        if (_p.x > target.x) _angle = -_angle;
    } else if (axis.y == -1) {
        if (_p.x < target.x) _angle = -_angle;
    }
}
void Transform::LookAt(Transform* target) { LookAt(target, Vec2::up); }

void Transform::LookAtLerp(Vec2 target, float t) {
    Vec2 axis = Vec2::up;
    float a = Vec2::Angle(axis, target - this->_p) * Mathf::Rad2Deg;
    // normalize
    if (axis.x == 1) {
        if (_p.y < target.y) a = -a;
    } else if (axis.x == -1) {
        if (_p.y > target.y) a = -a;
    }

    if (axis.y == 1) {
        if (_p.x > target.x) a = -a;
    } else if (axis.y == -1) {
        if (_p.x < target.x) a = -a;
    }

    _angle = Mathf::LerpAngle(_angle, a, t);
}
void Transform::LookAtLerp(Transform* target, float t) { LookAtLerp(target->_p, t); }

void Transform::LookAt(Transform* target, Vec2 axis) { LookAt(target->_p, axis); }

void Transform::as_first_child() {
    if (this->_parent == nullptr) return;
    hierarchy_sibiling(_parent, 0);  // 0 is first
}

void Transform::child_has(Transform* child) {}
void Transform::child_append(Transform* child) {
    Transform* t = this;
    hierarchy_append(t, child);
}
void Transform::child_remove(Transform* child) {
    Transform* t = this;
    hierarchy_remove(t, child);
}

const Vec2 Transform::forward() {
    static Vec2 axis = Vec2::up;
    Vec2 result = Vec2::RotateUp(_angle * Mathf::Deg2Rad, axis);
    result.Normalize();
    return result;
}
const Vec2 Transform::right() {
    Vec2 result;
    //TODO: Complete at
    return result;
}
const Vec2 Transform::left() {
    Vec2 result;
    //TODO: Complete at
    return result;
}
const Vec2 Transform::up() {
    Vec2 result;
    //TODO: Complete at
    return result;
}
const Vec2 Transform::down() {
    Vec2 result;
    //TODO: Complete at
    return result;
}

const Vec2 Transform::transformDirection(Vec2 direction) { return Vec2::RotateUp(_angle * Mathf::Deg2Rad, direction); }

const Vec2 Transform::transformDirection(float x, float y) { return transformDirection(Vec2(x, y)); }

const Vec2 Transform::rotate(Vec2 vec, Vec2 normal) {
    normal = Vec2::RotateUp(_angle * Mathf::Deg2Rad, normal);
    normal.x *= vec.x;
    normal.y *= vec.y;
    return normal;
}

Vec2 Transform::position() { return _p; }
void Transform::position(const Vec2& value) {
    _p = value;  // set the position
}
Vec2 Transform::localPosition() {
    if (this->_parent != nullptr) return this->_parent->_p - _p;
    return _p;
}
void Transform::localPosition(const Vec2& value) { _p = (this->_parent != nullptr) ? _parent->_p + value : value; }
const float Transform::angle() { return this->_angle; }
void Transform::angle(float value) {
    if (isnan(value)) value = 0;
    this->_angle = value;
}
const float Transform::localAngle() { return (this->_parent) ? this->_parent->_angle - this->_angle : this->_angle; }
void Transform::localAngle(float value) {}
Transform* Transform::parent() { return _parent; }
void Transform::setParent(Transform* parent) { hierarchy_parent_change(this, parent); }
void Transform::hierarchy_parent_change(Transform* from, Transform* newParent) {
    Transform* lastParent = from->_parent;

    if (newParent && lastParent == newParent) return;

    if (lastParent) {
        hierarchy_remove(lastParent, from);
    }

    if (!newParent)
        hierarchy_append(Level::getScene()->main_object->transform(),
                         from);  // nullptr as Root
    else
        from->_parent = newParent;
}

void Transform::hierarchy_remove(Transform* from, Transform* off) {
    if (off->_parent != from) return;

    from->hierarchy.remove(off);
    off->_parent = nullptr;  // not parent
}
void Transform::hierarchy_removeAll(Transform* from) {
    for (auto t : from->hierarchy) {
        t->_parent = nullptr;
    }

    from->hierarchy.clear();
}
void Transform::hierarchy_append(Transform* from, Transform* off) {
    auto iter = find_if(begin(from->hierarchy), end(from->hierarchy), [off](Transform* of) { return of == off; });
    if (iter == end(from->hierarchy)) {
        off->_parent = from;
        from->hierarchy.emplace_back(off);
    }
}
void Transform::hierarchy_sibiling(Transform* from, int index) {}
}  // namespace RoninEngine::Runtime
