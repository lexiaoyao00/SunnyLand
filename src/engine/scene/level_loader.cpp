#include "level_loader.h"
#include "../object/game_object.h"
#include "../component/sprite_component.h"
#include "../component/transform_component.h"
#include "../component/parallax_component.h"
#include "../component/tilelayer_component.h"
#include "../component/collider_component.h"
#include "../component/physics_component.h"
#include "../physics/collider.h"
#include "../scene/scene.h"
#include "../core/context.h"
#include "../utils/math.h"
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include <fstream>


namespace engine::scene {
    bool LevelLoader::loadLevel(const std::string &level_path, Scene *scene)
    {

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

        map_path_ = level_path;
        map_size_ = glm::ivec2(json_data.value("width", 0), json_data.value("height", 0));
        tile_size_ = glm::ivec2(json_data.value("tilewidth", 0), json_data.value("tileheight", 0));

        if (json_data.contains("tilesets") && json_data["tilesets"].is_array()) {
            for (const auto& tileset_json : json_data["tilesets"]) {
                if (!tileset_json.contains("source") || !tileset_json["source"].is_string() ||
                !tileset_json.contains("firstgid") || !tileset_json["firstgid"].is_number_unsigned())
                {
                    spdlog::error("Invalid tilesets: missing 'source' or 'firstgid' attribute");
                    continue;
                }
                auto tileset_path = resolvePath(tileset_json["source"].get<std::string>(), map_path_);
                auto first_gid = tileset_json["firstgid"].get<int>();
                loadTileset(tileset_path, first_gid);
            }
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
        auto texture_id = resolvePath(image_path,map_path_);

        const glm::vec2 offset = glm::vec2(layer_json.value("offsetx", 0.0f), layer_json.value("offsety", 0.0f));
        const glm::vec2 strcoll_factor = glm::vec2(layer_json.value("parallaxx", 1.0f), layer_json.value("parallaxy", 1.0f));
        const glm::bvec2 repeat = glm::bvec2(layer_json.value("repeatx", false), layer_json.value("repeaty", false));

        const std::string& layer_name = layer_json.value("name", "Unnamed");

        // more attributes...

        auto game_object = std::make_unique<engine::object::GameObject>(layer_name);
        game_object->addComponent<engine::component::TransformComponent>(offset);
        game_object->addComponent<engine::component::ParallaxComponent>(texture_id, strcoll_factor, repeat);

        scene->addGameObject(std::move(game_object));
        spdlog::info("Loaded image layer: {}", layer_name);
    }

    void LevelLoader::loadTileLayer(const nlohmann::json &layer_json, Scene *scene)
    {
        if (!layer_json.contains("data") || !layer_json["data"].is_array())
        {
            spdlog::error("Tile layer {} missing 'data' attribute", layer_json.value("name", "Unnamed"));
            return;
        }

        std::vector<engine::component::TileInfo> tiles;
        tiles.reserve(map_size_.x * map_size_.y);

        const auto& data = layer_json["data"];

        for (const auto& gid : data) {
            tiles.emplace_back(getTileInfoBtGid(gid));
        }

        const std::string& layer_name = layer_json.value("name", "Unnamed");
        auto game_object = std::make_unique<engine::object::GameObject>(layer_name);
        game_object->addComponent<engine::component::TileLayerComponent>(tile_size_, map_size_, std::move(tiles));

        scene->addGameObject(std::move(game_object));
        spdlog::info("Loaded tile layer: {}", layer_name);
    }

    void LevelLoader::loadObjectGroup(const nlohmann::json& layer_json, Scene* scene)
    {
        if(!layer_json.contains("objects") || !layer_json["objects"].is_array())
        {
            spdlog::error("Object group {} missing 'objects' attribute", layer_json.value("name", "Unnamed"));
            return;
        }

        const auto& objects = layer_json["objects"];
        for (const auto& object_json : objects)
        {
            auto gid = object_json.value("gid", 0);
            if (gid == 0)
            {
                // TODO: 自己绘制的形状，碰撞盒，触发器等，未来开发
            }
            else
            {
                auto tile_info = getTileInfoBtGid(gid);
                if (tile_info.sprite.getTextureId().empty())
                {
                    spdlog::error("Object gid {} not found in any tileset", gid);
                    continue;
                }

                auto position = glm::vec2(object_json.value("x", 0.0f), object_json.value("y", 0.0f));
                auto dst_size = glm::vec2(object_json.value("width", 0.0f), object_json.value("height", 0.0f));
                position = glm::vec2(position.x,position.y - dst_size.y); // 实际位置从左下角到左上角

                auto rotation = object_json.value("rotation", 0.0f);
                auto src_size_opt = tile_info.sprite.getSourceRect();
                if (!src_size_opt)
                {
                    spdlog::error("Object gid {} missing 'src_size' attribute", gid);
                    continue;
                }
                auto src_size = glm::vec2(src_size_opt->w, src_size_opt->h);
                auto scale = dst_size / src_size;

                const std::string& object_name = object_json.value("name", "Unnamed");

                auto game_object = std::make_unique<engine::object::GameObject>(object_name);
                game_object->addComponent<engine::component::TransformComponent>(position, scale, rotation);
                game_object->addComponent<engine::component::SpriteComponent>(std::move(tile_info.sprite),scene->getContext().getResourceManager());

                auto tile_json = getTileJsonByGid(gid);

                // 获取碰撞盒信息
                if (tile_info.type == engine::component::TileType::SOLID){  // 图集瓦片碰撞标签
                    auto collider = std::make_unique<engine::physics::AABBCollider>(src_size);
                    game_object->addComponent<engine::component::ColliderComponent>(std::move(collider));
                    game_object->addComponent<engine::component::PhysicsComponent>(&scene->getContext().getPhysicsEngine(),false);
                    game_object->setTag("solid");
                } else if (auto rect = getColliderRect(tile_json); rect){ // 对象瓦片自定义碰撞盒
                    auto collider = std::make_unique<engine::physics::AABBCollider>(rect->size);
                    auto* cc = game_object->addComponent<engine::component::ColliderComponent>(std::move(collider));
                    cc->setOffset(rect->position);  // 自定义碰撞盒的坐标是相对于图片坐标。
                    game_object->addComponent<engine::component::PhysicsComponent>(&scene->getContext().getPhysicsEngine(),false);
                }

                // 获取标签信息
                auto tag = getTileProperty<std::string>(tile_json, "tag");
                if (tag){
                    game_object->setTag(tag.value());
                }

                // 获取重力信息
                auto gravity = getTileProperty<bool>(tile_json, "gravity");
                if (gravity){
                    auto pc = game_object->getComponent<engine::component::PhysicsComponent>();
                    if (pc) {
                        pc->setUseGravity(gravity.value());
                    } else {
                        spdlog::warn("Object {} has gravity property but no physics component", object_name);
                        game_object->addComponent<engine::component::PhysicsComponent>(&scene->getContext().getPhysicsEngine(),false);
                    }
                }


                scene->addGameObject(std::move(game_object));
                spdlog::info("Loaded object: {}", object_name);
            }
        }
    }

    std::optional<engine::utils::Rect> LevelLoader::getColliderRect(const nlohmann::json &tile_json)
    {
        if (!tile_json.contains("objectgroup")) return std::nullopt;
        auto& objectgroup = tile_json["objectgroup"];
        if (!objectgroup.contains("objects")) return std::nullopt;
        auto& objects = objectgroup["objects"];
        for (const auto& object : objects) {    // 返回第一个不为空的碰撞矩形
            auto rect = engine::utils::Rect(glm::vec2(object.value("x",0.0f),object.value("y",0.0f)),glm::vec2(object.value("width",0.0f),object.value("height",0.0f)));
            if (rect.size.x > 0 && rect.size.y > 0) {
                return rect;
            }
        }
        return std::nullopt;
    }

    engine::component::TileType LevelLoader::getTileType(const nlohmann::json &tile_json)
    {
        if (tile_json.contains("properties")) {
            auto& properties = tile_json["properties"];
            for (const auto& property : properties) {
                if (property.contains("name") && property["name"] == "solid") {
                    bool is_solid = property.value("value", false);
                    return is_solid ? engine::component::TileType::SOLID : engine::component::TileType::NORMAL;
                }
                // TODO: 当前只支持 solid 属性，未来支持更多属性
            }
        }
        return engine::component::TileType::NORMAL;
    }

    engine::component::TileType LevelLoader::getTileTypeById(const nlohmann::json &tile_json, int local_id)
    {
        if (tile_json.contains("tiles")) {
            auto& tiles = tile_json["tiles"];
            for (auto& tile : tiles ){
                if (tile.contains("id") && tile["id"] == local_id) {
                    return getTileType(tile);
                }
            }
        }
        return engine::component::TileType::NORMAL;
    }

    engine::component::TileInfo LevelLoader::getTileInfoBtGid(int gid)
    {
        if (gid == 0)
        {
            return engine::component::TileInfo();
        }

        // upper_bound: 查找 tileset_data_ 中键大于 gid 的第一个元素，返回一个迭代器
        auto tileset_it = tilesets_data_.upper_bound(gid);
        if (tileset_it == tilesets_data_.begin())
        {
            spdlog::error("Tile gid {} not found in any tileset", gid);
            return engine::component::TileInfo();
        }
        --tileset_it;       // 前移一个位置，这样就得到不大于gid的最近的一个元素(我们需要的那个元素)

        const auto& tileset = tileset_it->second;
        auto local_id = gid - tileset_it->first;
        const std::string file_path = tileset.value("file_path", "");   // 获取图块集文件路径，在 loadTileset 时添加的
        if (file_path.empty()) {
            spdlog::error("Tileset file {} missing 'file_path' attribute", tileset_it->first);
            return engine::component::TileInfo();
        }

        if (tileset.contains("image"))  // 单一图片的情况（基于整张图块集图片）
        {
            auto texture_id = resolvePath(tileset["image"].get<std::string>(), file_path);

            // 计算瓦片在图片网格中的坐标
            auto coordinate_x = local_id % tileset["columns"].get<int>();
            auto coordinate_y = local_id / tileset["columns"].get<int>();

            SDL_FRect texture_rect = {
                static_cast<float>(coordinate_x * tile_size_.x),
                static_cast<float>(coordinate_y * tile_size_.y),
                static_cast<float>(tile_size_.x),
                static_cast<float>(tile_size_.y)
            };
            engine::render::Sprite sprite{texture_id, texture_rect};
            auto tile_type = getTileTypeById(tileset, local_id);
            return engine::component::TileInfo(sprite,tile_type);
        } else // 多图片的情况（多图片集合）
        {
            if (!tileset.contains("tiles"))
            {
                spdlog::error("Tileset {} missing 'tiles' attribute", tileset_it->first);
                return engine::component::TileInfo();
            }

            const auto& tiles_json = tileset["tiles"];
            for (const auto& tile_json : tiles_json)
            {
                auto tile_id = tile_json.value("id",0);
                if (tile_id == local_id)
                {
                    if (!tile_json.contains("image"))
                    {
                        spdlog::error("Tileset {} Tile {} missing 'image' attribute",tileset_it->first, tile_id);
                        return engine::component::TileInfo();
                    }

                    auto texture_id = resolvePath(tile_json["image"].get<std::string>(), file_path);
                    auto image_width = tile_json.value("imagewidth",0);
                    auto image_height = tile_json.value("imageheight",0);

                    SDL_FRect texture_rect = {
                        static_cast<float>(tile_json.value("x",0)),
                        static_cast<float>(tile_json.value("y",0)),
                        static_cast<float>(tile_json.value("width", image_width)),
                        static_cast<float>(tile_json.value("height", image_height))
                    };
                    engine::render::Sprite sprite{texture_id, texture_rect};
                    auto tile_type = getTileType(tile_json);
                    return engine::component::TileInfo(sprite, tile_type);
                }
            }
        }

        spdlog::error("Tile gid {} not found in tileset {}", gid, tileset_it->first);
        return engine::component::TileInfo();
    }

    std::optional<nlohmann::json> LevelLoader::getTileJsonByGid(int gid) const
    {
        auto tileset_it = tilesets_data_.upper_bound(gid);
        if (tileset_it == tilesets_data_.begin()){
            spdlog::error("Tile gid {} not found in any tileset", gid);
            return std::nullopt;
        }
        --tileset_it;

        const auto& tileset = tileset_it->second;
        auto local_id = gid - tileset_it->first;
        if (!tileset.contains("tiles")){
            spdlog::error("Tileset {} missing 'tiles' attribute", tileset_it->first);
            return std::nullopt;
        }

        const auto& tiles_josn = tileset["tiles"];
        for (const auto& tile_json : tiles_josn){
            auto tile_id = tile_json.value("id",0);
            if (tile_id == local_id){
                return tile_json;
            }
        }

        return std::nullopt;

    }

    void LevelLoader::loadTileset(const std::string &tileset_path, int first_gid)
    {
        std::ifstream tileset_file(tileset_path);
        if (!tileset_file.is_open()) {
            spdlog::error("Failed to open tileset file: {}", tileset_path);
            return;
        }

        nlohmann::json tileset_json;
        try
        {
            tileset_file >> tileset_json;
        }
        catch(const nlohmann::json::parse_error& e)
        {
            spdlog::error("Failed to parse tileset file: {}, error: {}(at byte {})", tileset_path, e.what(), e.byte);
            return;
        }
        tileset_json["file_path"] = tileset_path;       // 将文件路径存储到json中，后续解析图片路径时需要
        tilesets_data_[first_gid] = std::move(tileset_json);
        spdlog::info("Loaded tileset: {}, first gid: {}", tileset_path, first_gid);

    }

    std::string LevelLoader::resolvePath(const std::string &relative_path, const std::string &file_path)
    {
        try
        {
            auto map_dir = std::filesystem::path(file_path).parent_path();
            // std::filesystem::canonical 解析路径中的当前目录(.)和上级目录(..)导航符,得到一个干净的路径
            auto final_path = std::filesystem::canonical(map_dir / relative_path);
            return final_path.string();
        }
        catch(const std::exception& e)
        {
            spdlog::error("Failed to resolve path error: {}", e.what());
            return relative_path;
        }
    }

}   // namespace engine::scene