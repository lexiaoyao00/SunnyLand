#pragma once
#include <string>
#include <glm/vec2.hpp>
#include <nlohmann/json.hpp>
#include <map>
#include "../utils/math.h"

namespace engine::component {
    struct TileInfo;
    enum class TileType;
    class AnimationComponent;
    class AudioComponent;
}

namespace engine::scene {
class Scene;

class LevelLoader final {
    std::string map_path_;      // 地图路径（拼接路径时需要）
    glm::ivec2 map_size_;       // 地图尺寸（瓦块数量）
    glm::ivec2 tile_size_;     // 瓦块尺寸（像素）
    std::map<int, nlohmann::json> tilesets_data_; // firstgid -> 瓦片集数据
public:
    LevelLoader() = default;

    [[nodiscard]] bool loadLevel(const std::string& level_path, Scene* scene);

private:
    void loadImageLayer(const nlohmann::json& layer_json, Scene* scene);  // 加载图片层
    void loadTileLayer(const nlohmann::json& layer_json, Scene* scene);    // 加载瓦片层
    void loadObjectLayer(const nlohmann::json& layer_json, Scene* scene);  // 加载对象层


    /**
     * @brief 添加动画到指定的 AnimationComponent
     *
     * @param anim_json 动画json数据（自定义）
     * @param ac AnimationComponent指针（动画添加到此指针）
     * @param sprite_size 每一帧动画的尺寸
     */
    void addAnimation(const nlohmann::json& anim_json, engine::component::AnimationComponent* ac, const glm::vec2& sprite_size);

    void addSound(const nlohmann::json& sound_json, engine::component::AudioComponent* audio_component);

    /**
     * @brief 获取瓦片属性
     *
     * @tparam T 属性类型
     * @param tile_json 瓦片json数据
     * @param property_name 属性名称
     * @return std::optional<T> 属性值，如果属性不存在则返回 std::nullopt
     */
    template<typename T>
    std::optional<T> getTileProperty(const nlohmann::json& tile_json, const std::string& property_name) {
        if (!tile_json.contains("properties")) return std::nullopt;
        const auto& properties = tile_json["properties"];
        for (const auto& property : properties) {
            if (property.contains("name") && property["name"] == property_name) {
                if (property.contains("value")) {
                    return property["value"].get<T>();
                }
            }
        }
        return std::nullopt;

    }

    /**
     * @brief 获取瓦片碰撞矩形
     *
     * @param tile_json 瓦片json数据
     * @return std::optional<engine::utils::Rect> 碰撞器矩形，如果碰撞器不存在则返回 std::nullopt
     */
    std::optional<engine::utils::Rect> getColliderRect(const nlohmann::json& tile_json);


    /**
     * @brief 根据瓦片json对象获取瓦片类型
     *
     * @param tile_json 瓦片json数据
     * @return engine::component::TileType 瓦片类型
     */
    engine::component::TileType getTileType(const nlohmann::json& tile_json);

    /**
     * @brief 根据（单一图片）图块集中的 id 获取瓦片类型
     *
     * @param tile_json 图块集json数据
     * @param local_id 图块集中的 id
     * @return engine::component::TileType 瓦片类型
     */
    engine::component::TileType getTileTypeById(const nlohmann::json& tile_json, int local_id);

    /**
     * @brief 根据全局 id 获取瓦片信息
     *
     * @param gid 全局 id
     * @return engine::component::TileInfo 瓦片信息
     */
    engine::component::TileInfo getTileInfoByGid(int gid);

    /**
     * @brief 根据全局 ID 获取瓦片json对象（用于对象层获取瓦片信息）
     *
     * @param gid 全局 ID
     * @return std::optional<nlohmann::json> 瓦片json对象
     */
    std::optional<nlohmann::json> getTileJsonByGid(int gid) const;

    /**
     * @brief 加载 Tiled tileset 文件（.tsj）
     *
     * @param tileset_path Tileset 文件路径
     * @param first_gid 此 Tileset 的第一个全局 ID
     */
    void loadTileset(const std::string& tileset_path, int first_gid);

    /**
     * @brief 解析图片路径，合并地图路径和相对路径。例如
     * 1.地图路径： "assets/maps/level1.tmj"
     * 2.相对路径： "../textures/Layers/back.png"
     * 3.最终路径： "assets/textures/Layers/back.png"
     *
     * @param relative_path 相对路径（相对于文件）
     * @param file_path 文件路径
     * @return std::string 解析后的完整路径
     */
    std::string resolvePath(const std::string& relative_path, const std::string& file_path);

};

}   // namespace engine::scene