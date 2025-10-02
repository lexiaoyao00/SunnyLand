#include "player_component.h"
#include "../../engine/input/input_manager.h"
#include "../../engine/component/transform_component.h"
#include "../../engine/component/physics_component.h"
#include "../../engine/component/sprite_component.h"
#include "../../engine/component/animation_component.h"
#include "../../engine/component/health_component.h"
#include "../../engine/component/audio_component.h"
#include "../../engine/object/game_object.h"
#include "state/idle_state.h"
#include "state/hurt_state.h"
#include "state/dead_state.h"


#include <utility>
#include <typeinfo>
#include <spdlog/spdlog.h>


namespace game::component {
    bool PlayerComponent::takeDamage(int damage)
    {
        if (is_dead_ || !health_component_ || damage <= 0){
            spdlog::warn("玩家已经死亡或缺少必要组件，无法承受伤害");
            return false;
        }

        bool success = health_component_->takeDamage(damage);
        if (!success)  return false;

        if (health_component_->isAlive()) {
            spdlog::debug("玩家受到{}点伤害，当前生命值：{}/{}。",
                          damage, health_component_->getCurrentHealth(), health_component_->getMaxHealth());
            // 切换到受伤状态
            setState(std::make_unique<state::HurtState>(this));
        } else {
            spdlog::debug("玩家死亡。");
            is_dead_ = true;
            // 切换到死亡状态
            setState(std::make_unique<state::DeadState>(this));
        }
        return true;
    }

void PlayerComponent::setState(std::unique_ptr<state::PlayerState> new_state)
{
    if (!new_state) {
        spdlog::warn("Tried to set null state");
        return;
    }
    if (current_state_){
        current_state_->exit();
    }

    current_state_ = std::move(new_state);
    spdlog::debug("Set player state to {}", typeid(*current_state_).name());
    current_state_->enter();

}

bool PlayerComponent::isOnGround() const
{
    return coyote_timer_ <= coyote_time_ || physics_component_->hasCollidedBelow();
}

void PlayerComponent::init()
{
    if (!owner_) {
        spdlog::error("PlayerComponent has no owner");
        return;
    }

    // 获取必要组件
    transform_component_ = owner_->getComponent<engine::component::TransformComponent>();
    physics_component_ = owner_->getComponent<engine::component::PhysicsComponent>();
    sprite_component_ = owner_->getComponent<engine::component::SpriteComponent>();
    animation_component_ = owner_->getComponent<engine::component::AnimationComponent>();
    health_component_ = owner_->getComponent<engine::component::HealthComponent>();
    audio_component_ = owner_->getComponent<engine::component::AudioComponent>();

    if (!transform_component_ || !physics_component_ || !sprite_component_ ||
        !animation_component_ || !health_component_ || !audio_component_) {
        spdlog::error("PlayerComponent missing required components");
    }

    // 初始化状态机
    current_state_ = std::make_unique<state::IdleState>(this);
    if (current_state_) {
        setState(std::move(current_state_));
    } else {
        spdlog::error("Failed to initialize player state");
    }

    spdlog::debug("PlayerComponent initialized");
}

void PlayerComponent::handleInput(engine::core::Context &context)
{
    if (!current_state_) {
        spdlog::error("PlayerComponent has no current state");
        return;
    }

    auto next_state = current_state_->handleInput(context);
    if (next_state) {
        setState(std::move(next_state));
    }
}

void PlayerComponent::update(float delta_time, engine::core::Context &context)
{
    if (!current_state_) {
        spdlog::error("PlayerComponent has no current state");
        return;
    }
    // 一旦离地，开始计时 Coyote timer
    if (!physics_component_->hasCollidedBelow()) {
        coyote_timer_ += delta_time;
    } else {
        coyote_timer_ = 0.0f;
    }

    // 无敌时间，进行闪烁
    if (health_component_->isInvincible()){
        flash_timer_ += delta_time;
        if (flash_timer_ >= 2*flash_interval_) {
            flash_timer_ -= 2*flash_interval_;    // 闪烁计时器在 0~2 倍闪烁间隔 中循环
        }

        if (flash_timer_ < flash_interval_) {
            sprite_component_->setHidden(true);
        } else {
            sprite_component_->setHidden(false);
        }
    }
    // 非无敌状态时确保精灵可见
    else if (sprite_component_->isHidden()) {
        sprite_component_->setHidden(false);
    }

    auto next_state = current_state_->update(delta_time, context);
    if (next_state) {
        setState(std::move(next_state));
    }
}

} // namespace game::component
