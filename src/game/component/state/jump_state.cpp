#include "jump_state.h"
#include "idle_state.h"
#include "walk_state.h"
#include "fall_state.h"
#include "../../../engine/core/context.h"
#include "../../../engine/component/physics_component.h"
#include "../../../engine/component/sprite_component.h"
#include "../../../engine/input/input_manager.h"
#include "../player_component.h"

#include <spdlog/spdlog.h>
#include <glm/glm.hpp>


namespace game::component::state{
    void JumpState::enter()
    {
        auto physics_component = player_component_->getPhysicsComponent();
        physics_component->velocity_.y = - player_component_->getJumpForce(); // 向上跳
        spdlog::debug("JumpState::enter, velocity.y = {}", physics_component->velocity_.y);
    }

    void JumpState::exit()
    {
    }

    std::unique_ptr<PlayerState> JumpState::handleInput(engine::core::Context &context)
    {
        auto input_manager = context.getInputManager();
        auto physics_component = player_component_->getPhysicsComponent();
        auto sprite_component = player_component_->getSpriteComponent();

        // 如果按下 "jump" 则切换到 JumpState
        if (input_manager.isActionPressed("jump")) {
            return std::make_unique<JumpState>(player_component_);
        }

        // 步行状态可以左右移动
        if (input_manager.isActionDown("move_left")){
            if (physics_component->velocity_.x > 0.0f){
                physics_component->velocity_.x = 0.0f;      // 先减速到0，增强手感
            }
            physics_component->addForce({-player_component_->getMoveForce(), 0.0f});
            sprite_component->setFlipped(true); // 向左移动时翻转精灵图
        } else if (input_manager.isActionDown("move_right")){
            if (physics_component->velocity_.x < 0.0f){
                physics_component->velocity_.x = 0.0f;
            }
            physics_component->addForce({player_component_->getMoveForce(), 0.0f});
            sprite_component->setFlipped(false); // 向右移动时恢复精灵图
        }

        return nullptr;
    }

    std::unique_ptr<PlayerState> JumpState::update(float , engine::core::Context &)
    {
        // 限制最大速度（水平方向）
        auto physics_component = player_component_->getPhysicsComponent();
        auto max_speed = player_component_->getMaxSpeed();
        physics_component->velocity_.x = glm::clamp(physics_component->velocity_.x, -max_speed, max_speed);

        // 如果速度为正，切换到 FallState
        if (physics_component->velocity_.y >= 0.0f) {
            return std::make_unique<FallState>(player_component_);
        }

        return nullptr;
    }

} // namespace game::component::state