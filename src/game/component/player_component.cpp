#include "player_component.h"
#include "../../engine/input/input_manager.h"
#include "../../engine/component/transform_component.h"
#include "../../engine/component/physics_component.h"
#include "../../engine/component/sprite_component.h"
#include "../../engine/component/animation_component.h"
#include "../../engine/object/game_object.h"
#include "state/idle_state.h"


#include <utility>
#include <typeinfo>
#include <spdlog/spdlog.h>


namespace game::component {
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

    if (!transform_component_ || !physics_component_ || !sprite_component_ || !animation_component_) {
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

    auto next_state = current_state_->update(delta_time, context);
    if (next_state) {
        setState(std::move(next_state));
    }
}

} // namespace game::component
