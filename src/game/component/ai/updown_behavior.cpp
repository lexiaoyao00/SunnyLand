#include "updown_behavior.h"

#include "../ai_component.h"
#include "../../../engine/component/animation_component.h"
#include "../../../engine/component/physics_component.h"
#include "../../../engine/component/transform_component.h"
#include "../../../engine/component/sprite_component.h"

#include <spdlog/spdlog.h>

namespace game::component::ai {
UpDownBehavior::UpDownBehavior(float min_y, float max_y, float speed)
    : patrol_min_y_(min_y), patrol_max_y_(max_y), move_speed_(speed)
{
    if (patrol_min_y_ >= patrol_max_y_) {
        spdlog::error("UpDownBehavior: patrol_min_y_ should be less than patrol_max_y_");
        patrol_min_y_ = patrol_max_y_;  // 修正修为，避免逻辑错误
    }
}

void UpDownBehavior::enter(AIComponent &ai_component)
{
    if (auto* animation_component = ai_component.getAnimationComponent(); animation_component) {
        animation_component->playAnimation("fly");
    }

    if (auto* physics_component = ai_component.getPhysicsComponent(); physics_component) {
        physics_component->setUseGravity(false);
    }
}

void UpDownBehavior::update(float, AIComponent &ai_component)
{
    auto* physics_component = ai_component.getPhysicsComponent();
    auto* transform_component = ai_component.getTransformComponent();

    if (!physics_component || !transform_component) {
        spdlog::error("UpDownBehavior: 缺少必要的组件，无法执行巡逻行为。");
        return;
    }

    auto current_y = transform_component->getPosition().y;

    if (physics_component->hasCollidedAbove() || current_y <= patrol_min_y_) {
        physics_component->velocity_.y = move_speed_;
        move_down_ = true;
    } else if (physics_component->hasCollidedBelow() || current_y >= patrol_max_y_) {
        physics_component->velocity_.y = -move_speed_;
        move_down_ = false;
    }

    // 不需要反转精灵图
}

} // namespace game::component::ai
