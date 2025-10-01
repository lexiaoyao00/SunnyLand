#include "patrol_behavior.h"
#include "../ai_component.h"
#include "../../../engine/component/animation_component.h"
#include "../../../engine/component/physics_component.h"
#include "../../../engine/component/transform_component.h"
#include "../../../engine/component/sprite_component.h"

#include <spdlog/spdlog.h>

namespace game::component::ai {

PatrolBehavior::PatrolBehavior(float min_x, float max_x, float speed)
    : patrol_min_x_(min_x),
      patrol_max_x_(max_x),
      move_speed_(speed)
{
    if (patrol_min_x_ >= patrol_max_x_) {
        spdlog::error("PatrolBehavior: patrol_min_x_ must be less than patrol_max_x_");
        patrol_min_x_ = patrol_max_x_;  // 修正修为，避免逻辑错误
    }
}
void PatrolBehavior::enter(AIComponent &ai_component)
{
    if (auto* animationi_component = ai_component.getAnimationComponent(); animationi_component) {
        animationi_component->playAnimation("walk");
    }
}
void PatrolBehavior::update(float, AIComponent &ai_component)
{
    auto* physics_component = ai_component.getPhysicsComponent();
    auto* transform_component = ai_component.getTransformComponent();
    auto* sprite_component = ai_component.getSpriteComponent();
    if (!physics_component || !transform_component || !sprite_component) {
        spdlog::error("PatrolBehavior: missing required components");
        return;
    }

    auto current_x = transform_component->getPosition().x;

    if (physics_component->hasCollidedRight() || current_x >= patrol_max_x_) {
        physics_component->velocity_.x = -move_speed_;
        move_right_ = false;
    } else if (physics_component->hasCollidedLeft() || current_x <= patrol_min_x_) {
        physics_component->velocity_.x = move_speed_;
        move_right_ = true;
    }

    sprite_component->setFlipped(move_right_);
}
} // namespace game::component::ai
