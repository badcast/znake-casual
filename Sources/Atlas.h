#pragma once

#include "framework.h"

namespace RoninEngine::Runtime {

   class Atlas {
         friend GC;
         Texture* texture;
         std::map<string, Sprite*> _sprites;

      public:
         Atlas() = default;
         Atlas(const Atlas&) = default;
         ~Atlas() = default;

         Sprite* get(int pos);

         Sprite* Get_Sprite(const string& spriteName);
   };

}  // namespace RoninEngine::Runtime
