#include "walk_state.h"
#include "jump_state.h"
#include "idle_state.h"
#include "fall_state.h"
#include "climb_state.h"
#include "../../../engine/core/context.h"
#include "../../../engine/component/physics_component.h"
#include "../../../engine/component/sprite_component.h"
#include "../../../engine/input/input_manager.h"
#include "../player_component.h"

#include <glm/glm.hpp>

namespace game::component::state{
    void WalkState::enter()
    {
        playAnimation("walk");
    }

    void WalkState::exit()
    {

    }

    std::unique_ptr<PlayerState> WalkState::handleInput(engine::core::Context& context)
    {
        auto input_manager = context.getInputManager();
        auto physics_component = player_component_->getPhysicsComponent();
        auto sprite_component = player_component_->getSpriteComponent();

        if (physics_component->hasCollidedLadder() && input_manager.isActionDown("move_up") ){
                return std::make_unique<ClimbState>(player_component_);
        }

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
        } else {
            // 如果没有左右移动输入，则切换到 IdleState
            return std::make_unique<IdleState>(player_component_);
        }

        return nullptr;
    }

    std::unique_ptr<PlayerState> WalkState::update(float , engine::core::Context &)
    {
        // 限制最大速度（水平方向）
        auto physics_conmonent = player_component_->getPhysicsComponent();
        auto max_speed = player_component_->getMaxSpeed();
        physics_conmonent->velocity_.x = glm::clamp(physics_conmonent->velocity_.x, -max_speed, max_speed);

        // 如果下方没有碰撞，则切换到 FallState
        if (!player_component_->isOnGround()){
            return std::make_unique<FallState>(player_component_);
        }

        return nullptr;
    }

}   // namespace game::component::state
