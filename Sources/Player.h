#pragma once
#include "Behaviour.h"
#include "Spotlight.h"
#include "framework.h"
#include "inputSystem.h"
#include "time.h"

namespace RoninEngine::Runtime {
class Player : public Behaviour {
public:
  Camera2D *playerCamera;
  SpriteRenderer *spriteRenderer;
  Spotlight *spotlight;
  Player() : Player(typeid(*this).name()) {}
  Player(const string &name) : Behaviour(name) {}
  float speed = 0.05f;

  void OnAwake() override;
  void OnStart() override;
  void OnUpdate() override;
  void OnLateUpdate() override;
  void OnDestroy() override;
};
} // namespace RoninEngine::Runtime
