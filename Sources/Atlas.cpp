#include "pch.h"
#include "Atlas.h"

namespace RoninEngine::Runtime {
	Sprite* Atlas::Get_Sprite(const string& spriteName) {
		auto iter = _sprites.find(spriteName);
		if (iter == end(_sprites))
			return nullptr; 
          return iter->second;
	}
}
