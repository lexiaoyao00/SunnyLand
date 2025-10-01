#include "ai_component.h"
#include "../../engine/component/health_component.h"
#include "../../engine/component/transform_component.h"
#include "../../engine/component/physics_component.h"
#include "../../engine/component/animation_component.h"
#include "../../engine/component/sprite_component.h"
#include "../../engine/object/game_object.h"
#include <spdlog/spdlog.h>

namespace game::component {
void AIComponent::setBehavior(std::unique_ptr<ai::AIBehavior> behavior)
{
    current_behavior_ = std::move(behavior);
    spdlog::debug("GameObject {} set new behavior",owner_ ? owner_->getName() : "Unknown");
    if (current_behavior_) {
        current_behavior_->enter(*this);
    }
}

bool AIComponent::takeDamage(int damage)
{
    bool success = false;
    if (auto* health_component = getOwner()->getComponent<engine::component::HealthComponent>(); health_component) {
        success = health_component->takeDamage(damage);
        // TODO:设置受伤/死亡后的行为
    }
    return success;
}

bool AIComponent::isAlive() const
{
    if (auto* health_component = getOwner()->getComponent<engine::component::HealthComponent>(); health_component) {
        return health_component->isAlive();
    }
    return true;    // 没有生命组件，默认返回存活状态
}

void AIComponent::init()
{
    if (!owner_){
        spdlog::error("AIComponent 没有所属游戏对象");
        return;
    }

    transform_component_ = owner_->getComponent<engine::component::TransformComponent>();
    physics_component_ = owner_->getComponent<engine::component::PhysicsComponent>();
    animation_component_ = owner_->getComponent<engine::component::AnimationComponent>();
    sprite_component_ = owner_->getComponent<engine::component::SpriteComponent>();

    if (!transform_component_ || !physics_component_ || !animation_component_ || !sprite_component_) {
        spdlog::error("GameObject {} 上的 AIComponent 缺少必须的组件",owner_ ? owner_->getName() : "Unknown");
    }
}

void AIComponent::update(float delta_time, engine::core::Context &)
{
    if (current_behavior_) {
        current_behavior_->update(delta_time, *this);
    } else {
        spdlog::warn("GameObject {} 上的 AIComponent 没有设置行为",owner_ ? owner_->getName() : "Unknown");
    }
}

}   // namespace game::component

