#pragma once

#include "framework.h"

namespace RoninEngine::Runtime {

   class Behaviour :
         public Component
   {
      public:
         Behaviour() : Behaviour("Behaviour") {}
         Behaviour(const string& name) : Component(name) {}
         Behaviour(const Behaviour&) = delete;
         Behaviour(Behaviour&&) = delete;
         virtual ~Behaviour() = default;

         virtual void OnAwake() {}
         virtual void OnStart() {}
         virtual void OnUpdate() {}
         virtual void OnLateUpdate() {}
         virtual void OnDestroy() {}
   };
}

