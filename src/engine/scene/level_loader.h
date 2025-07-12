#pragma once
#include <string>
#include <nlohmann/json_fwd.hpp>



namespace engine::scene {
class Scene;

class LevelLoader final {
    std::string map_path_;      // 地图路径（拼接路径时需要）
public:
    LevelLoader() = default;

    bool loadLevel(const std::string& level_path, Scene* scene);

private:
    void loadImageLayer(const nlohmann::json& layer_json, Scene* scene);  // 加载图片层
    void loadTileLayer(const nlohmann::json& layer_json, Scene* scene);    // 加载瓦片层
    void loadObjectGroup(const nlohmann::json& layer_json, Scene* scene);  // 加载对象层

    /**
     * @brief 解析图片路径，合并地图路径和相对路径。例如
     * 1.地图路径： "assets/maps/level1.tmj"
     * 2.相对路径： "../textures/Layers/back.png"
     * 3.最终路径： "assets/textures/Layers/back.png"
     *
     * @param image_path 图片相对路径
     * @return std::string 解析后的完整路径
     */
    std::string resolvePath(std::string image_path);

};

}   // namespace engine::scene