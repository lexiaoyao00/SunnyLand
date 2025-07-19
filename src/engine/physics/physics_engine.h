#pragma once
#include <vector>
#include <glm/vec2.hpp>

namespace engine::component {
    class PhysicsComponent;
}

namespace engine::physics {

class PhysicsEngine {
private:
    std::vector<component::PhysicsComponent*> components_;    // 注册过的物理组件容器，非拥有指针
    glm::vec2 gravity_ = {0.0f, 980.0f};    // 默认重力值(像素/秒^2,相当于100像素对应于1米)
    float max_speed_ = 500.0f;    // 最大速度(像素/秒)

public:
    PhysicsEngine() = default;

    PhysicsEngine(const PhysicsEngine&) = delete;
    PhysicsEngine& operator=(const PhysicsEngine&) = delete;
    PhysicsEngine(PhysicsEngine&&) = delete;
    PhysicsEngine& operator=(PhysicsEngine&&) = delete;

    void registerComponent(component::PhysicsComponent* component);
    void unregisterComponent(component::PhysicsComponent* component);

    void update(float delta_time);

    void setGravity(const glm::vec2& gravity) { gravity_ = gravity; }
    const glm::vec2& getGravity() const { return gravity_; }
    void setMaxSpeed(float max_speed) { max_speed_ = max_speed; }
    float getMaxSpeed() const { return max_speed_; }

};

}   // namespace engine::physics