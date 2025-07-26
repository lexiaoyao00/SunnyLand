#include "tilelayer_component.h"
#include "../core/context.h"
#include "../render/renderer.h"
#include "../render/camera.h"
#include "../physics/physics_engine.h"
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>

namespace engine::component {

    TileLayerComponent::TileLayerComponent(glm::ivec2 tile_size, glm::ivec2 map_size, std::vector<TileInfo> &&tiles)
    : tile_size_(tile_size), map_size_(map_size), tiles_(std::move(tiles))
    {
        if (tiles_.size() != static_cast<size_t>(map_size_.x * map_size_.y)) {
            spdlog::error("TileLayerComponent: map size does not match tile count,data will be clear");
            tiles_.clear();
            map_size_ = {0, 0};
        }
        spdlog::trace("TileLayerComponent created");
    }

    const TileInfo *TileLayerComponent::getTileInfoAt(glm::ivec2 pos) const
    {
        if (pos.x < 0 || pos.x >= map_size_.x || pos.y < 0 || pos.y >= map_size_.y)
        {
            spdlog::warn("TileLayerComponent: position out of range: ({}, {})", pos.x, pos.y);
            return nullptr;
        }

        size_t index = static_cast<size_t>(pos.y * map_size_.x + pos.x);
        if (index < tiles_.size())
        {
            return &tiles_[index];
        }

        spdlog::warn("TileLayerComponent: index out of range: {}", index);
        return nullptr;
    }

    TileType TileLayerComponent::getTileTypeAt(glm::ivec2 pos) const
    {
        const TileInfo* info = getTileInfoAt(pos);
        return info ? info->type : TileType::EMPTY;
    }

    TileType TileLayerComponent::getTileTypeAtWorldPos(const glm::vec2 &world_pos) const
    {
        glm::vec2 relative_pos = world_pos - offset_;   // 像素坐标

        int tile_x = static_cast<int>(std::floor(relative_pos.x / tile_size_.x));
        int tile_y = static_cast<int>(std::floor(relative_pos.y / tile_size_.y));

        return getTileTypeAt(glm::ivec2{tile_x, tile_y});
    }

    void TileLayerComponent::init()
    {
        if (!owner_)
        {
            spdlog::warn("TileLayerComponent: owner is null");
        }
        spdlog::trace("TileLayerComponent initalized");
    }

    void TileLayerComponent::render(engine::core::Context &context)
    {
        if (tile_size_.x <= 0 || tile_size_.y <= 0)
        {
            spdlog::warn("TileLayerComponent: tile size is invalid");
            return;
        }

        for (int y = 0; y < map_size_.y; ++y)
        {
            for (int x = 0; x < map_size_.x; ++x)
            {
                size_t index = static_cast<size_t>(y) * map_size_.x + x;
                if (index < tiles_.size() && tiles_[index].type != TileType::EMPTY)
                {
                    const auto& tile_info = tiles_[index];

                    glm::vec2 tile_left_top_pos = {
                        offset_.x + static_cast<float>(x) * tile_size_.x,
                        offset_.y + static_cast<float>(y) * tile_size_.y
                    };

                    // 如果图片大小与瓦片的大小不一致，需要调整 y 坐标（瓦片层的对齐点时左下角。大图需要向上偏移坐标渲染）
                    if (static_cast<int>(tile_info.sprite.getSourceRect()->h) != tile_size_.y)
                    {
                        tile_left_top_pos.y -= (tile_info.sprite.getSourceRect()->h - tile_size_.y);
                    }
                    context.getRenderer().drawSprite(context.getCamera(), tile_info.sprite, tile_left_top_pos);
                }
            }
        }
    }

    void TileLayerComponent::clean()
    {
        if (physics_engine_)
        {
            physics_engine_->unregisterCollisionTileLayer(this);
        }
    }
}
