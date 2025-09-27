#include "hurt_state.h"
#include "idle_state.h"
#include "walk_state.h"
#include "fall_state.h"
#include "../../../engine/component/sprite_component.h"
#include "../../../engine/component/physics_component.h"
#include "../player_component.h"

#include <glm/glm.hpp>

namespace game::component::state {

    void HurtState::enter()
    {
        playAnimation("hurt");
        auto physics_component = player_component_->getPhysicsComponent();
        auto sprite_component = player_component_->getSpriteComponent();
        auto knockback_velocity = glm::vec2(-100.0f, -150.0f);  // 默认左上方击退效果

        // 根据当前精灵图朝向状态决定是否改成右上方
        if (sprite_component->isFlipped()){
            knockback_velocity.x = -knockback_velocity.x;   // 变成向右
        }
        physics_component->velocity_ = knockback_velocity;
    }

    void HurtState::exit()
    {
    }

    std::unique_ptr<PlayerState> HurtState::handleInput(engine::core::Context &)
    {
        // 僵直期不能进行操作
        return nullptr;
    }

    std::unique_ptr<PlayerState> HurtState::update(float delta_time, engine::core::Context &)
    {
        stunned_timer_ += delta_time;

        // 两种情况离开受伤（僵直）状态
        // 1. 落地
        auto physics_component = player_component_->getPhysicsComponent();
        if (physics_component->hasCollidedBelow()) {
            if (glm::abs(physics_component->velocity_.x < 1.0f)){
                return std::make_unique<IdleState>(player_component_);
            } else {
                return std::make_unique<WalkState>(player_component_);
            }
        }

        // 2. 僵直时间结束
        if (stunned_timer_ > player_component_->getStunnedDuration()) {
            stunned_timer_ = 0.0f;
            return std::make_unique<FallState>(player_component_);
        }
        return nullptr;
    }

}   // namespace game::component::state


