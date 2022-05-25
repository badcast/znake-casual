#pragma once
#include "Component.h"
#include "framework.h"

namespace RoninEngine::Runtime {
class Transform : public Component {
    friend class RoninEngine::Level;
    friend class Camera;
    friend class Camera2D;
    friend GameObject* Instantiate(GameObject* obj);

    protected:
    list<Transform*> hierarchy;
    Transform* _parent;
    Vec2 _p;
    float _angle;

    static void hierarchy_parent_change(Transform* from, Transform* newParent);
    static void hierarchy_remove(Transform* from, Transform* off);
    static void hierarchy_removeAll(Transform* from);
    static void hierarchy_append(Transform* from, Transform* off);
    static void hierarchy_sibiling(Transform* from, int index);

   public:
    Transform();
    Transform(const string& name);
    ~Transform();

    Transform* parent();
    void setParent(Transform* parent);

    int child_count();
    Transform* child_of(int index);

    void LookAt(Vec2 target, Vec2 axis);
    void LookAt(Vec2 target);
    void LookAt(Transform* target, Vec2 axis);
    void LookAt(Transform* target);
    void LookAtLerp(Vec2 target, float t);
    void LookAtLerp(Transform* target, float t);

    void as_first_child();

    void child_has(Transform* child);
    void child_append(Transform* child);
    void child_remove(Transform* child);

    const Vec2 forward();
    const Vec2 right();
    const Vec2 left();
    const Vec2 up();
    const Vec2 down();

    const Vec2 transformDirection(Vec2 direction);
    const Vec2 transformDirection(float x, float y);

    const Vec2 rotate(Vec2 vec, Vec2 normal);

    //Точка в мировых кординатах
    Vec2 position();
    void position(const Vec2& value);

    //Точка относительно в кординатах родителя
    Vec2 localPosition();
    void localPosition(const Vec2& value);

    const float angle();
    void angle(float value);
    const float localAngle();
    void localAngle(float value);
};

}  // namespace RoninEngine::Runtime
