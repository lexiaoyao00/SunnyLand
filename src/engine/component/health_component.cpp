#include "health_component.h"
#include "../object/game_object.h"

#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

namespace engine::component {

    HealthComponent::HealthComponent(int max_health, float invincible_durantion)
        : max_health_(glm::max(1, max_health))
        , current_health_(max_health)
        , invincible_durantion_(invincible_durantion)
    {}

    bool HealthComponent::takeDamage(int damage_amount)
    {
        if (damage_amount <= 0 || !isAlive()){
            return false;   // 不造成伤害或已经死亡
        }

        if (is_invincible_){
            spdlog::debug("游戏对象 '{}' 在无敌状态，免疫了 {} 点伤害。", owner_ ? owner_->getName() : "Unkown", damage_amount);
            return false;
        }

        current_health_ -= damage_amount;
        current_health_ = glm::max(0, current_health_);

        // 受伤没死触发无敌时间
        if (isAlive() && invincible_durantion_ > 0.0f) {
            setInvincible(invincible_durantion_);
        }

        spdlog::debug("游戏对象 '{}' 受到了 {} 点伤害，剩余 {} 点生命值。", owner_ ? owner_->getName() : "Unkown", damage_amount, current_health_);
        return true;
    }

    void HealthComponent::heal(int heal_amount)
    {
        if (heal_amount <= 0 || !isAlive()){
            return;   // 不造成治疗或已经死亡
        }

        current_health_ += heal_amount;
        current_health_ = glm::min(max_health_, current_health_);

        spdlog::debug("游戏对象 '{}' 获得了 {} 点治疗，剩余 {} 点生命值。", owner_ ? owner_->getName() : "Unkown", heal_amount, current_health_);
    }

    void HealthComponent::setCurrentHealth(int current_health)
    {
        current_health_ = glm::max(0, glm::min(current_health, max_health_));
    }

    void HealthComponent::setMaxHealth(int max_health)
    {
        max_health_ = glm::max(1, max_health);
        current_health_ = glm::min(max_health_, current_health_);
    }

    void HealthComponent::setInvincible(float durantion)
    {
        if (durantion > 0.0f)
        {
            is_invincible_ = true;
            invincible_timer_ = durantion;
            spdlog::debug("游戏对象 '{}' 进入无敌状态，持续 {} 秒。", owner_ ? owner_->getName() : "Unkown", durantion);
        } else {
            is_invincible_ = false;
            invincible_timer_ = 0.0f;
            spdlog::debug("游戏对象 '{}' 退出无敌状态。", owner_ ? owner_->getName() : "Unkown");
        }
    }

    void HealthComponent::update(float delta_time, engine::core::Context &)
    {
        if (is_invincible_){
            invincible_timer_ -= delta_time;
            if (invincible_timer_ <= 0.0f)
            {
                is_invincible_ = false;
                invincible_timer_ = 0.0f;
            }
        }
    }

} // namespace engine::component
