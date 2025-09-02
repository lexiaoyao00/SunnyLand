#pragma once
#include <vector>
#include <glm/vec2.hpp>

namespace engine::component {
    class PhysicsComponent;
    class TileLayerComponent;
}

namespace engine::object {
    class GameObject;
}

namespace engine::physics {

class PhysicsEngine {
private:
    std::vector<engine::component::PhysicsComponent*> components_;    // 注册过的物理组件容器，非拥有指针
    std::vector<engine::component::TileLayerComponent*> collision_tile_layers_;     // 注册过的 碰撞瓦片图层 容器
    glm::vec2 gravity_ = {0.0f, 980.0f};    // 默认重力值(像素/秒^2,相当于100像素对应于1米)
    float max_speed_ = 500.0f;    // 最大速度(像素/秒)

    /// @brief 存储本帧法师的 GameObject 碰撞对 （每次 update 开始时清空）
    std::vector<std::pair<engine::object::GameObject*, engine::object::GameObject*>> collision_pairs_;
public:
    PhysicsEngine() = default;

    PhysicsEngine(const PhysicsEngine&) = delete;
    PhysicsEngine& operator=(const PhysicsEngine&) = delete;
    PhysicsEngine(PhysicsEngine&&) = delete;
    PhysicsEngine& operator=(PhysicsEngine&&) = delete;

    void registerComponent(component::PhysicsComponent* component);
    void unregisterComponent(component::PhysicsComponent* component);

    // 如果瓦片层需要进行碰撞检测则注册，不需要则不注册
    void registerCollisionTileLayer(component::TileLayerComponent* tile_layer);
    void unregisterCollisionTileLayer(component::TileLayerComponent* tile_layer);

    void update(float delta_time);

    void setGravity(const glm::vec2& gravity) { gravity_ = gravity; }
    const glm::vec2& getGravity() const { return gravity_; }
    void setMaxSpeed(float max_speed) { max_speed_ = max_speed; }
    float getMaxSpeed() const { return max_speed_; }
    const std::vector<std::pair<engine::object::GameObject*, engine::object::GameObject*>>& getCollisionPairs() const {
        return collision_pairs_;
    }

private:
    void checkObjectCollision();
    void resolveTileCollision(engine::component::PhysicsComponent* pc, float delta_time);   // 检测并处理游戏对象和瓦片层之间的碰撞
    void resolveSolidObjectCollision(engine::object::GameObject* move_obj, engine::object::GameObject* solid_obj);   // 检测可移动物体与SOLID物体的碰撞

};

}   // namespace engine::physics