#pragma once
#include "framework.h"
#include "Object.h"
#include "UI.h"

namespace RoninEngine {
   class Level {
         friend class Application;
         friend class UI::GUI;
         friend class Runtime::Object;
         friend class Runtime::GameObject;
         friend class Runtime::Renderer;
         friend class Runtime::Camera;

         friend bool Runtime::instanced(Runtime::Object* obj);
         friend void Runtime::Destroy(Runtime::Object* obj);
         friend void Runtime::Destroy(Runtime::Object* obj, float t);
         friend void Runtime::Destroy_Immediate(Runtime::Object* obj);

      private:
         int globalID;
         bool m_isUnload;
         string m_name;
         std::list<Runtime::Behaviour*>* _firstRunScripts;
         std::list<Runtime::Behaviour*>* _realtimeScripts;
         std::list<pair<Runtime::Object*, float>>* _destructions;

         std::list<Runtime::Renderer*> _assoc_renderers;
         std::list<Runtime::Light*> _assoc_lightings;

         std::map<Runtime::Object*, float> _objects;

         void PinScript(Runtime::Behaviour* obj);
         void CC_Render_Push(Runtime::Renderer* rend);
         void CC_Light_Push(Runtime::Light* light);
         void ObjectPush(Runtime::Object* obj);

      protected:
         UI::GUI* ui;
         virtual void RenderScene(SDL_Renderer* renderer);
         virtual void RenderUI(SDL_Renderer* renderer);
         virtual void RenderSceneLate(SDL_Renderer* renderer);

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

         virtual void awake() = 0;
         virtual void start() = 0;
         virtual void update() = 0;
         virtual void lateUpdate() = 0;
         virtual void onDrawGizmos() = 0;
         virtual void onUnloading() = 0;

         static Level* getScene();
   };

}  // namespace RoninEngine
