#include "level_loader.h"
#include "../object/game_object.h"
#include "../component/transform_component.h"
#include "../component/parallax_component.h"
#include "../scene/scene.h"
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include <fstream>


namespace engine::scene {
    bool LevelLoader::loadLevel(const std::string &level_path, Scene *scene)
    {
        map_path_ = level_path;

        std::ifstream file(level_path);
        if (!file.is_open()) {
            spdlog::error("Failed to open level file: {}", level_path);
            return false;
        }

        nlohmann::json json_data;
        try
        {
            file >> json_data;
        }
        catch(const nlohmann::json::parse_error& e)
        {
            spdlog::error("Failed to parse level file: {}, error: {}", level_path, e.what());
            return false;
        }

        if (!json_data.contains("layers") || !json_data["layers"].is_array()) {
            spdlog::error("Invalid level file: {}, missing layers or not an array", level_path);
            return false;
        }

        for (const auto& layer_json : json_data["layers"]) {
            std::string layer_type = layer_json.value("type", "none");
            if (!layer_json.value("visible",true)){
                spdlog::info("Layer '{}' is visible", layer_json.value("name", "Unnamed"));
                continue;
            }

            if (layer_type == "imagelayer"){
                loadImageLayer(layer_json, scene);
            } else if (layer_type == "tilelayer") {
                loadTileLayer(layer_json, scene);
            } else if (layer_type == "objectgroup") {
                loadObjectGroup(layer_json, scene);
            } else
            {
                spdlog::warn("Unsupported layer type: {}", layer_type);
            }
        }

        return false;
    }

    void LevelLoader::loadImageLayer(const nlohmann::json &layer_json, Scene *scene)
    {
        const std::string& image_path = layer_json.value("image", "");
        if (image_path.empty()) {
            spdlog::error("Image layer {} without 'image' attribute", layer_json.value("name", "Unnamed"));
            return;
        }
        auto texture_id = resolvePath(image_path);

        const glm::vec2 offset = glm::vec2(layer_json.value("offsetx", 0.0f), layer_json.value("offsety", 0.0f));
        const glm::vec2 strcoll_factor = glm::vec2(layer_json.value("parallaxx", 1.0f), layer_json.value("parallaxy", 1.0f));
        const glm::bvec2 repeat = glm::bvec2(layer_json.value("repeatx", false), layer_json.value("repeaty", false));

        const std::string& layer_name = layer_json.value("name", "Unnamed");

        // TODO: more attributes

        auto game_object = std::make_unique<engine::object::GameObject>(layer_name);
        game_object->addComponent<engine::component::TransformComponent>(offset);
        game_object->addComponent<engine::component::ParallaxComponent>(texture_id, strcoll_factor, repeat);

        scene->addGameObject(std::move(game_object));
        spdlog::info("Loaded image layer: {}", layer_name);
    }

    void LevelLoader::loadTileLayer(const nlohmann::json &, Scene *)
    {
        // TODO:
    }

    void LevelLoader::loadObjectGroup(const nlohmann::json &, Scene *)
    {
        // TODO:
    }

    std::string LevelLoader::resolvePath(std::string image_path)
    {
        try
        {
            auto map_dir = std::filesystem::path(map_path_).parent_path();
            // std::filesystem::canonical 解析路径中的当前目录(.)和上级目录(..)导航符,得到一个干净的路径
            auto final_path = std::filesystem::canonical(map_dir / image_path);
            return final_path.string();
        }
        catch(const std::exception& e)
        {
            spdlog::error("Failed to resolve path: {}, error: {}", image_path, e.what());
            return image_path;
        }


    }

}   // namespace engine::scene