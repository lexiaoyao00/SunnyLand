#include "jump_behavior.h"
#include "../ai_component.h"
#include "../../../engine/component/animation_component.h"
#include "../../../engine/component/physics_component.h"
#include "../../../engine/component/transform_component.h"
#include "../../../engine/component/sprite_component.h"

#include <spdlog/spdlog.h>


namespace game::component::ai{


JumpBehavior::JumpBehavior(float min_x, float max_x, glm::vec2 jump_vel, float jump_interval)
    : patrol_min_x_(min_x),
      patrol_max_x_(max_x),
      jump_vel_(jump_vel),
      jump_interval_(jump_interval)
{

}

void JumpBehavior::update(float delta_time, AIComponent &ai_component)
{
    auto* physics_component = ai_component.getPhysicsComponent();
    auto* transform_component = ai_component.getTransformComponent();
    auto* animation_component = ai_component.getAnimationComponent();
    auto* sprite_component = ai_component.getSpriteComponent();

    if (!physics_component || !transform_component || !animation_component || !sprite_component) {
        spdlog::error("JumpBehavior: 缺少必要的组件，无法执行跳跃行为");
        return;
    }

    auto is_on_ground = physics_component->hasCollidedBelow();
    if (is_on_ground) {
        jump_timer_ += delta_time;
        physics_component->velocity_.x = 0.0f;

        if (jump_timer_ >= jump_interval_) {
            jump_timer_ = 0.0f;

            auto current_x = transform_component->getPosition().x;
            if (jumping_right_ && (physics_component->hasCollidedRight() || current_x >= patrol_max_x_)) {
                jumping_right_ = false;
            } else if (!jumping_right_ && (physics_component->hasCollidedLeft() || current_x <= patrol_min_x_)) {
                jumping_right_ = true;
            }
            auto jump_vel_x = jumping_right_ ? jump_vel_.x : -jump_vel_.x;
            physics_component->velocity_ = glm::vec2(jump_vel_x, jump_vel_.y);
            animation_component->playAnimation("jump");
            sprite_component->setFlipped(jumping_right_);

        } else {
            animation_component->playAnimation("idle");
        }
    } else {
            if (physics_component->getVelocity().y < 0){
                animation_component->playAnimation("jump");
            } else {
                animation_component->playAnimation("fall");
            }
        }
}

}   // namespace game::component::ai
