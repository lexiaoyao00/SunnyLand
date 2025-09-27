#pragma once
#include "component.h"

namespace engine::component {

class HealthComponent : public Component {
    friend class engine::object::GameObject;

private:
    int max_health_ = 1;
    int current_health_ = 1;
    bool is_invincible_ = false; //是否处于无敌状态
    float invincible_durantion_ = 0.0f; //无敌状态持续的时间（秒）
    float invincible_timer_ = 0.0f; //无敌状态计时器（秒）

public:
    explicit HealthComponent(int max_health = 1, float invincible_durantion = 2.0f);
    ~HealthComponent() override = default;

    HealthComponent(const HealthComponent&) = delete;
    HealthComponent& operator=(const HealthComponent&) = delete;
    HealthComponent(HealthComponent&&) = delete;
    HealthComponent& operator=(HealthComponent&&) = delete;

    bool takeDamage(int damage_amount); // 受到伤害
    void heal(int heal_amount);         // 治疗总额

    // getters and setters
    bool isAlive() const { return current_health_ > 0; }
    bool isInvincible() const { return is_invincible_; }
    int getCurrentHealth() const { return current_health_; }
    int getMaxHealth() const { return max_health_; }

    void setCurrentHealth(int current_health);  // 设置当前生命中（确保不大于最大生命值）
    void setMaxHealth(int max_health);          // 设置最大生命值（确保不小于1）
    void setInvincible(float durantion);        // 进入无敌状态，持续一定时间
    void setInvincibleDuration(float durantion) { invincible_durantion_ = durantion; }    // 设置无敌状态持续时间

protected:
    void update(float,engine::core::Context&) override;
};

}   // namespace engine::component