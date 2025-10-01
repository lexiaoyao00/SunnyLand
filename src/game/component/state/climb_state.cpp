#include "climb_state.h"
#include "jump_state.h"
#include "idle_state.h"
#include "fall_state.h"

#include "../player_component.h"
#include "../../../engine/input/input_manager.h"
#include "../../../engine/core/context.h"
#include "../../../engine/component/physics_component.h"
#include "../../../engine/component/animation_component.h"
#include "../../../engine/object/game_object.h"
#include <spdlog/spdlog.h>
#include <glm/common.hpp>

namespace game::component::state {
    void ClimbState::enter()
    {
        spdlog::debug("进入攀爬状态");
        playAnimation("climb");
        if (auto* physics_component = player_component_->getPhysicsComponent(); physics_component){
            physics_component->setUseGravity(false);
        }
    }

    void ClimbState::exit()
    {
        spdlog::debug("退出攀爬状态");
        if (auto* physics_component = player_component_->getPhysicsComponent(); physics_component){
            physics_component->setUseGravity(true);
        }
    }

    std::unique_ptr<PlayerState> ClimbState::handleInput(engine::core::Context &context)
    {
        auto input_manager = context.getInputManager();
        auto physics_component = player_component_->getPhysicsComponent();
        auto aniimation_component = player_component_->getAnimationComponent();

        // 攀爬状态下，按键则移动，不按键则静止
        auto is_up = input_manager.isActionDown("move_up");
        auto is_down = input_manager.isActionDown("move_down");
        auto is_left = input_manager.isActionDown("move_left");
        auto is_right = input_manager.isActionDown("move_right");
        auto speed = player_component_->getClimbSpeed();

        physics_component->velocity_.y = is_up ? -speed : is_down ? speed : 0;
        physics_component->velocity_.x = is_left ? -speed : is_right ? speed : 0;

        // 根据是否有按键决定动画播放情况
        (is_up || is_down || is_left || is_right) ? aniimation_component->resumeAnimation() : aniimation_component->stopAnimation();

        // 按跳跃键主动离开攀爬状态
        if (input_manager.isActionDown("jump")) {
            return std::make_unique<JumpState>(player_component_);
        }
        return nullptr;
    }

    std::unique_ptr<PlayerState> ClimbState::update(float, engine::core::Context &)
    {
        auto physics_component = player_component_->getPhysicsComponent();
        // 如果着地，则切换到 IdleState
        if (physics_component->hasCollidedBelow()){
            return std::make_unique<IdleState>(player_component_);
        }
        // 如果离开梯子区域，则切换到 FallState （能走到这里说明非着地状态）
        if (!physics_component->hasCollidedLadder()){
            return std::make_unique<FallState>(player_component_);
        }

        return nullptr;
    }

}   // namespace game::component::state