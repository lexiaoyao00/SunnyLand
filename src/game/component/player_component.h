#pragma once
#include "../../engine/component/component.h"
#include "state/player_state.h"
#include <memory>

namespace engine::input {
    class InputManager;
}

namespace engine::component {
    class TransformComponent;
    class PhysicsComponent;
    class SpriteComponent;
    class AnimationComponent;
    class HealthComponent;
}

namespace game::component::state {
    class PlayerState;
}

namespace game::component {
class PlayerComponent : public engine::component::Component {
    friend class engine::object::GameObject;

private:
    engine::component::TransformComponent* transform_component_ = nullptr;  // 指向 TransformComponent 的非拥有指针
    engine::component::PhysicsComponent* physics_component_ = nullptr;  // 指向 PhysicsComponent 的非拥有指针
    engine::component::SpriteComponent* sprite_component_ = nullptr;  // 指向 SpriteComponent 的非拥有指针
    engine::component::AnimationComponent* animation_component_ = nullptr;  // 指向 AnimationComponent 的非拥有指针
    engine::component::HealthComponent* health_component_ = nullptr;  // 指向 HealthComponent 的非拥有指针

    std::unique_ptr<state::PlayerState> current_state_;
    bool is_dead_ = false;

    // ---移动相关参数
    float move_force_ = 200.0f;      // 水平移动力
    float max_speed_ = 120.0f;      // 最大速度（像素/秒）
    float jump_vel_ = 350.0f;      // 跳跃速度（按下 "jump" 时的瞬时向上的速度
    float friction_factor_ = 0.85f;     // 摩擦系数（ Idle 时的缓冲效果，每帧乘以此系数）

    // ---属性相关参数
    float stunned_duration_ = 0.4f;  // 玩家被击中后的僵直时间（秒）

public:
    PlayerComponent() = default;
    ~PlayerComponent() override = default;

    PlayerComponent(const PlayerComponent&) = delete;
    PlayerComponent& operator=(const PlayerComponent&) = delete;
    PlayerComponent(PlayerComponent&&) = delete;
    PlayerComponent& operator=(PlayerComponent&&) = delete;

    bool takeDamage(int damage);  // 试图造成伤害，返回是否成功

    // setters and getters
    engine::component::TransformComponent* getTransformComponent() const { return transform_component_; }
    engine::component::PhysicsComponent* getPhysicsComponent() const { return physics_component_; }
    engine::component::SpriteComponent* getSpriteComponent() const { return sprite_component_; }
    engine::component::AnimationComponent* getAnimationComponent() const { return animation_component_; }
    engine::component::HealthComponent* getHealthComponent() const { return health_component_; }

    void setIsDead(bool is_dead) { is_dead_ = is_dead; }
    bool isDead() const { return is_dead_; }
    void setMoveForce(float move_force) { move_force_ = move_force; }
    void setMaxSpeed(float max_speed) { max_speed_ = max_speed; }
    void setJumpVelocity(float jump_force) { jump_vel_ = jump_force; }
    void setFrictionFactor(float friction_factor) { friction_factor_ = friction_factor; }
    void setStunnedDuration(float stunned_duration) { stunned_duration_ = stunned_duration; }

    float getMoveForce() const { return move_force_; }
    float getMaxSpeed() const { return max_speed_; }
    float getJumpVelocity() const { return jump_vel_; }
    float getFrictionFactor() const { return friction_factor_; }
    float getStunnedDuration() const { return stunned_duration_; }

    void setState(std::unique_ptr<state::PlayerState> new_state);  // 切换玩家状态

private:

    void init() override;  // 初始化组件
    void handleInput(engine::core::Context& context) override;  // 处理输入
    void update(float delta_time, engine::core::Context& context) override;  // 更新组件

};  // class PlayerComponent
}  // namespace game::component