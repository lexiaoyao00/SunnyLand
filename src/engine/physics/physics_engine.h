#pragma once
#include <vector>
#include <glm/vec2.hpp>
#include <optional>
#include "../utils/math.h"

namespace engine::component {
    class PhysicsComponent;
    class TileLayerComponent;
    enum class TileType;
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
    std::optional<engine::utils::Rect> world_bounds_;   // 世界边界

    /// @brief 存储本帧发生的 GameObject 碰撞对 （每次 update 开始时清空）
    std::vector<std::pair<engine::object::GameObject*, engine::object::GameObject*>> collision_pairs_;
    /// @brief 存储本帧发生的瓦片触发事件（每次 update 开始时清空）
    std::vector<std::pair<engine::object::GameObject*, engine::component::TileType>> tile_trigger_events_;
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
    void setWorldBounds(const engine::utils::Rect& world_bounds) { world_bounds_ = world_bounds; }
    const std::optional<engine::utils::Rect>& getWorldBounds() const { return world_bounds_; }
    const std::vector<std::pair<engine::object::GameObject*, engine::object::GameObject*>>& getCollisionPairs() const {
        return collision_pairs_;
    }
    const std::vector<std::pair<engine::object::GameObject*, engine::component::TileType>>& getTileTriggerEvents() const {
        return tile_trigger_events_;
    }

private:
    void checkObjectCollision();    // 物体间碰撞检测
    void resolveTileCollision(engine::component::PhysicsComponent* pc, float delta_time);   // 检测并处理游戏对象和瓦片层之间的碰撞
    void resolveSolidObjectCollision(engine::object::GameObject* move_obj, engine::object::GameObject* solid_obj);   // 检测可移动物体与SOLID物体的碰撞

    void applyWorldBounds(engine::component::PhysicsComponent* pc);    // 应用世界边界，限制物体移动范围


    /**
     * @brief 根据瓦片类型盒指定宽度x坐标，计算瓦片上对应的y坐标
     *
     * @param width 从瓦片左侧起算的宽度
     * @param type 瓦片宽度
     * @param tile_size 瓦片尺寸
     * @return float 瓦片上对应的高度（从瓦片下侧起算）
     */
    float getTileHeightAtWidth(float width, engine::component::TileType type, glm::vec2 tile_size);

    /**
     * @brief 检测所有游戏对象与瓦片层的触发类型瓦片碰撞，并记录触发事件。（位移处理完毕后再调用）
     *
     */
    void checkTileTriggers();

};

}   // namespace engine::physics