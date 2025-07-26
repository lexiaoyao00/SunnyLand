#pragma once
#include <string>
#include <glm/vec2.hpp>
#include <nlohmann/json.hpp>
#include <map>

namespace engine::component {
    struct TileInfo;
    enum class TileType;
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

    bool loadLevel(const std::string& level_path, Scene* scene);

private:
    void loadImageLayer(const nlohmann::json& layer_json, Scene* scene);  // 加载图片层
    void loadTileLayer(const nlohmann::json& layer_json, Scene* scene);    // 加载瓦片层
    void loadObjectGroup(const nlohmann::json& layer_json, Scene* scene);  // 加载对象层

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
    engine::component::TileInfo getTileInfoBtGid(int gid);

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