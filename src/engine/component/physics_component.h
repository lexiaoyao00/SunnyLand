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

private:
    void init() override;
    void update(float, engine::core::Context&) override {}
    void clean() override;
};

}   // namespace engine::component