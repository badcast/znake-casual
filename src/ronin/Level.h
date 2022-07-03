#pragma once
#include "dependency.h"

namespace RoninEngine {
class Level {
    friend class Application;
    friend class UI::GUI;
    friend class Runtime::Object;
    friend class Runtime::Transform;
    friend class Runtime::GameObject;
    friend class Runtime::Renderer;
    friend class Runtime::Camera;
    friend class Runtime::Camera2D;

    friend bool Runtime::instanced(Runtime::Object* obj);
    friend void Runtime::Destroy(Runtime::Object* obj);
    friend void Runtime::Destroy(Runtime::Object* obj, float t);
    friend void Runtime::Destroy_Immediate(Runtime::Object* obj);

   private:
    int globalID;
    bool m_isUnload;
    std::string m_name;
    std::list<Runtime::Behaviour*>* _firstRunScripts;
    std::list<Runtime::Behaviour*>* _realtimeScripts;
    std::list<std::pair<Runtime::Object*, float>>* _destructions;
    std::map<float, std::set<Runtime::Transform*>> matrixWorld;

    std::list<Runtime::Renderer*> _assoc_renderers;
    std::list<Runtime::Light*> _assoc_lightings;

    std::map<Runtime::Object*, float> _objects;
    std::list<Runtime::GameObject*> _gameObjects;

    void PinScript(Runtime::Behaviour* obj);
    void CC_Render_Push(Runtime::Renderer* rend);
    void CC_Light_Push(Runtime::Light* light);
    void ObjectPush(Runtime::Object* obj);

    std::vector<Runtime::Transform*>* getInternalTransforms(Runtime::Transform* parent);

   protected:
    UI::GUI* ui;
    virtual void RenderLevel(SDL_Renderer* renderer);
    virtual void RenderUI(SDL_Renderer* renderer);
    virtual void RenderSceneLate(SDL_Renderer* renderer);
    void callback_movement(Runtime::Transform* transform, Runtime::Vec2 lastPoint);

   public:
    // Main or Root object
    Runtime::GameObject* main_object;

    Level();
    Level(const std::string& name);

    virtual ~Level();

    std::string& name();

    bool is_hierarchy();

    UI::GUI* Get_GUI();
    void Unload();

    virtual void awake() {}
    virtual void start() {}
    virtual void update() {}
    virtual void lateUpdate() {}
    virtual void onDrawGizmos() {}
    virtual void onUnloading() {}
    static std::list<Runtime::Transform *> hierarhy_damage_checker();
    static Level* getScene();
};

}  // namespace RoninEngine
