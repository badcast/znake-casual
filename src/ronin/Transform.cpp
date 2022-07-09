#include "Transform.h"

#include "framework.h"

namespace RoninEngine::Runtime {

Transform::Transform() : Transform(typeid(*this).name()) {}

Transform::Transform(const std::string& name) : Component(name) {
    _parent = nullptr;
    _angle = 0;
    // set as default
    Level::self()->matrix_nature(this, Vec2::RoundToInt(p + Vec2::one));
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
    int x;

    if (child_count() <= index || child_count() >= index) throw std::out_of_range("index");

    auto iter = begin(hierarchy);
    for (x = 0; iter != end(hierarchy); ++x) {
        if (x == index) {
            tf = *iter;
            break;
        }
    }

    return tf;
}
void Transform::LookAt(Vec2 target, Vec2 axis) {
    _angle = Vec2::Angle(axis, target - p) * Mathf::Rad2Deg;

    // normalize horz
    if (axis.x == 1) {
        if (p.y < target.y) _angle = -_angle;
    } else if (axis.x == -1) {
        if (p.y > target.y) _angle = -_angle;
    }
    // normalize vert
    if (axis.y == 1) {
        if (p.x > target.x) _angle = -_angle;
    } else if (axis.y == -1) {
        if (p.x < target.x) _angle = -_angle;
    }
}

void Transform::LookAt(Transform* target) { LookAt(target, Vec2::up); }

void Transform::LookAt(Transform* target, Vec2 axis) { LookAt(target->p, axis); }

void Transform::LookAt(Vec2 target) { LookAt(target, Vec2::up); }

void Transform::LookAtLerp(Vec2 target, float t) {
    Vec2 axis = Vec2::up;
    float a = Vec2::Angle(axis, target - p) * Mathf::Rad2Deg;
    // normalize
    if (axis.x == 1) {
        if (p.y < target.y) a = -a;
    } else if (axis.x == -1) {
        if (p.y > target.y) a = -a;
    }

    if (axis.y == 1) {
        if (p.x > target.x) a = -a;
    } else if (axis.y == -1) {
        if (p.x < target.x) a = -a;
    }

    _angle = Mathf::LerpAngle(_angle, a, t);
}

void Transform::LookAtLerp(Transform* target, float t) { LookAtLerp(target->p, t); }

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

const Vec2 Transform::forward() { return transformDirection(Vec2::up); }

const Vec2 Transform::right() { return transformDirection(Vec2::right); }

const Vec2 Transform::left() { return transformDirection(Vec2::left); }

const Vec2 Transform::up() { return transformDirection(Vec2::up); }

const Vec2 Transform::down() { return transformDirection(Vec2::down); }

const Vec2 Transform::transformDirection(Vec2 direction) { return Vec2::RotateUp(_angle * Mathf::Deg2Rad, direction); }

const Vec2 Transform::transformDirection(float x, float y) { return transformDirection(Vec2(x, y)); }

const Vec2 Transform::rotate(Vec2 vec, Vec2 normal) {
    normal = Vec2::RotateUp(_angle * Mathf::Deg2Rad, normal);
    normal.x *= vec.x;
    normal.y *= vec.y;
    return normal;
}

Vec2 Transform::position() { return p; }
void Transform::position(const Vec2& value) {
    Vec2Int lastPoint = Vec2::RoundToInt(p);
    if(p==value)
        return;
    p = value;  // set the position
    Level::self()->matrix_nature(this, lastPoint);
    for (Transform* chlid : hierarchy) chlid->parent_notify();
}
Vec2 Transform::localPosition() {
    if (this->_parent != nullptr) return this->_parent->p - p;
    return p;
}
void Transform::localPosition(const Vec2& value) {
    Vec2Int lastPoint = Vec2::RoundToInt(p);
    p = (this->_parent != nullptr) ? _parent->p + value : value;
    Level::self()->matrix_nature(this, lastPoint);
}

float Transform::angle() { return this->_angle; }

void Transform::angle(float value) { this->_angle = value; }

float Transform::localAngle() {
    float langle = (this->_parent) ? this->_parent->_angle + this->_angle : this->_angle;
    return langle;
}
void Transform::localAngle(float value) {
    // TODO: create local angle
    throw std::runtime_error("Not implemented");
}
Transform* Transform::parent() { return _parent; }

void Transform::setParent(Transform* parent) { hierarchy_parent_change(this, parent); }

void Transform::parent_notify() {
    Vec2 newPosition = _parent->p - p;
    position(newPosition);
}

void Transform::hierarchy_parent_change(Transform* from, Transform* newParent) {
    Transform* lastParent = from->_parent;

    if (newParent && lastParent == newParent) return;

    if (lastParent) {
        hierarchy_remove(lastParent, from);
    }

    if (!newParent)
        hierarchy_append(Level::self()->main_object->transform(),
                         from);  // nullptr as Root
    else {
        from->_parent = newParent;
        hierarchy_append(newParent, from);
    }
}

void Transform::hierarchy_remove(Transform* from, Transform* off) {
    if (off->_parent != from) return;

    auto iter = std::find(from->hierarchy.begin(), from->hierarchy.end(), off);
    if (iter == from->hierarchy.end()) return;
    from->hierarchy.erase(iter);
    from->hierarchy.shrink_to_fit();
    off->_parent = nullptr;  // not parent
}
void Transform::hierarchy_removeAll(Transform* from) {
    for (auto t : from->hierarchy) {
        t->_parent = nullptr;
    }

    from->hierarchy.clear();
    from->hierarchy.shrink_to_fit();
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
