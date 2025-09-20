#pragma once
#include "../render/sprite.h"
#include "component.h"
#include <vector>
#include <glm/vec2.hpp>

namespace engine::render {
    class Sprite;
}

namespace engine::core {
    class Context;
}

namespace engine::physics {
    class PhysicsEngine;
}

namespace engine::component {

enum class TileType {
    EMPTY,          // 空白瓦片
    NORMAL,         // 普通瓦片
    SOLID,          // 静止可碰撞瓦片
    UNISOLID,       // 单向静止可碰撞瓦片
    SLOPE_0_1,      // 斜坡瓦片，高度：左0  右1
    SLOPE_1_0,      // 斜坡瓦片，高度：左1  右0
    SLOPE_0_2,      // 斜坡瓦片，高度：左0  右1/2
    SLOPE_2_0,      // 斜坡瓦片，高度：左1/2  右0
    SLOPE_1_2,      // 斜坡瓦片，高度：左1  右1/2
    SLOPE_2_1,      // 斜坡瓦片，高度：左1/2  右1
    // ...
};

struct TileInfo
{
    render::Sprite sprite;      // 瓦片的视觉表示
    TileType type;              // 瓦片类型

    TileInfo(render::Sprite s = render::Sprite(), TileType t = TileType::EMPTY) : sprite(std::move(s)), type(t) {}
};

class TileLayerComponent final : public Component
{
    friend class engine::object::GameObject;

private:
    glm::ivec2 tile_size_;      // 单个瓦片尺寸（像素）
    glm::ivec2 map_size_;       // 地图尺寸（瓦片数）
    std::vector<TileInfo> tiles_;   // 所有瓦片信息（按照 行主序 存储，index = y * map_width + x）
    glm::vec2 offset_ = {0.0f, 0.0f};  // 瓦片层在世界中的偏移量

    bool is_hidden_ = false;  // 是否隐藏瓦片层
    engine::physics::PhysicsEngine* physics_engine_ = nullptr;  // 物理引擎指针, clean() 函数中可能需要反注册

public:
    TileLayerComponent() = default;
    TileLayerComponent(glm::ivec2 tile_size, glm::ivec2 map_size, std::vector<TileInfo>&& tiles);

    const TileInfo* getTileInfoAt(glm::ivec2 pos) const;
    TileType getTileTypeAt(glm::ivec2 pos) const;
    TileType getTileTypeAtWorldPos(const glm::vec2& world_pos) const;

    // getters
    const glm::ivec2 getTileSize() const { return tile_size_; }
    const glm::ivec2 getMapSize() const { return map_size_; }
    const glm::vec2 getWorldSize() const { return glm::vec2(map_size_.x * tile_size_.x, map_size_.y * tile_size_.y); }
    const std::vector<TileInfo>& getTiles() const { return tiles_; }
    const glm::vec2& getOffset() const { return offset_; }
    bool isHidden() const { return is_hidden_; }

    // setters
    void setOffset(const glm::vec2& offset) { offset_ = offset; }
    void setHidden(bool hidden) { is_hidden_ = hidden; }
    void setPhysicsEngine(engine::physics::PhysicsEngine* physics_engine) { physics_engine_ = physics_engine; }

protected:
    void init() override;
    void update(float,engine::core::Context&) override {}
    void render(engine::core::Context& context) override;
    void clean() override;
};



}   // namespace engine::component