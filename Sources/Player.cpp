#include "pch.h"
#include "Player.h"
#include "Camera2D.h"
#include "SpriteRenderer.h"

namespace RoninEngine::Runtime {

    void Player::OnAwake() {
        playerCamera = gameObject()->Add_Component<Camera2D>();
        spriteRenderer = gameObject()->Add_Component<SpriteRenderer>();
        spotlight = gameObject()->Add_Component<Spotlight>();
    }

    void Player::OnStart() {

    }

    void Player::OnUpdate() {
        float curSpeed = input::get_key(SDL_SCANCODE_LSHIFT) ? (speed * 10) : speed;
        if(playerCamera)
        {

        auto cameraPoint = playerCamera->transform()->position();
        auto point = cameraPoint;
        playerCamera->transform()->position(Vec2::MoveTowards(point, point + input::get_axis(), curSpeed));
        transform()->LookAtLerp(playerCamera->ScreenToWorldPoint(input::getMousePointF()), .05f);
        }
    }

    void Player::OnLateUpdate() {

    }
    void Player::OnDestroy() {

    }

}
