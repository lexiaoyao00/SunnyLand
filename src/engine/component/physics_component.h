#pragma once
#include "component.h"
#include <glm/vec2.hpp>

namespace engine::physics {
    class PhysicsEngine;
}

namespace engine::component {
class TransformComponent;

class PhysicsComponent final : public Component{
    friend class engine::object::GameObject;
public:
    glm::vec2 velocity_ = {0.0f, 0.0f};         // 物体的速度，设置为公共成员变量方便 PhysicsEngine 调用

private:
    engine::physics::PhysicsEngine* physics_engine_ = nullptr;
    TransformComponent* transform_ = nullptr;

    glm::vec2 force_ = {0.0f, 0.0f};             // 当前帧物体的受力
    float mass_ = 1.0f;                           // 物体的质量
    bool use_gravity_ = true;                        // 物体是否受重力影响
    bool enable_ = true;                          // 物体是否启用物理引擎


    // 碰撞状态标志
    bool collided_below_ = false;
    bool collided_above_ = false;
    bool collided_left_ = false;
    bool collided_right_ = false;
    bool collided_ladder_ = false;
    bool is_on_top_ladder_ = false;

public:
    PhysicsComponent(engine::physics::PhysicsEngine* physics_engine, bool use_gravity = true, float mass = 1.0f);
    ~PhysicsComponent() override = default;

    PhysicsComponent(const PhysicsComponent&) = delete;
    PhysicsComponent& operator=(const PhysicsComponent&) = delete;
    PhysicsComponent(PhysicsComponent&&) = delete;
    PhysicsComponent& operator=(PhysicsComponent&&) = delete;

    void addForce(const glm::vec2& force) { if (enable_) force_ += force; }
    void clearForce() { force_ = {0.0f, 0.0f}; }
    const glm::vec2& getForce() const { return force_; }
    float getMass() const { return mass_; }
    bool isEnable() const { return enable_; }
    bool isUseGravity() const { return use_gravity_; }

    void setEnable(bool enable) { enable_ = enable; }
    void setMass(float mass) { mass_ = (mass >= 0.0f) ? mass : 1.0f; }
    void setUseGravity(bool use_gravity) { use_gravity_ = use_gravity; }
    void setVelocity(const glm::vec2& velocity) { velocity_ = velocity; }
    const glm::vec2& getVelocity() const { return velocity_; }
    TransformComponent* getTransform() const { return transform_; }


    // ------- 碰撞状态访问与修改（供 physicsEngine 使用）------------
    void resetCollisionFlags(){
        collided_below_ = false;
        collided_above_ = false;
        collided_left_ = false;
        collided_right_ = false;
        collided_ladder_ = false;
        is_on_top_ladder_ = false;
    }

    void setCollidedBelow(bool collided) { collided_below_ = collided; }
    void setCollidedAbove(bool collided) { collided_above_ = collided; }
    void setCollidedLeft(bool collided) { collided_left_ = collided; }
    void setCollidedRight(bool collided) { collided_right_ = collided; }
    void setCollidedLadder(bool collided) { collided_ladder_ = collided; }
    void setOnTopLadder(bool on_top_ladder) { is_on_top_ladder_ = on_top_ladder; }

    bool hasCollidedBelow() const { return collided_below_; }
    bool hasCollidedAbove() const { return collided_above_; }
    bool hasCollidedLeft() const { return collided_left_; }
    bool hasCollidedRight() const { return collided_right_; }
    bool hasCollidedLadder() const { return collided_ladder_; }
    bool isOnTopLadder() const { return is_on_top_ladder_; }

private:
    void init() override;
    void update(float, engine::core::Context&) override {}
    void clean() override;
};

}   // namespace engine::component