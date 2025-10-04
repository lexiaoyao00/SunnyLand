#include "level_loader.h"
#include "../object/game_object.h"
#include "../component/sprite_component.h"
#include "../component/transform_component.h"
#include "../component/parallax_component.h"
#include "../component/tilelayer_component.h"
#include "../component/collider_component.h"
#include "../component/physics_component.h"
#include "../component/animation_component.h"
#include "../component/health_component.h"
#include "../component/audio_component.h"
#include "../render/animation.h"
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
                loadObjectLayer(layer_json, scene);
            } else
            {
                spdlog::warn("Unsupported layer type: {}", layer_type);
            }
        }

        return true;
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
            tiles.emplace_back(getTileInfoByGid(gid));
        }

        const std::string& layer_name = layer_json.value("name", "Unnamed");
        auto game_object = std::make_unique<engine::object::GameObject>(layer_name);
        game_object->addComponent<engine::component::TileLayerComponent>(tile_size_, map_size_, std::move(tiles));

        scene->addGameObject(std::move(game_object));
        spdlog::info("Loaded tile layer: {}", layer_name);
    }

    void LevelLoader::loadObjectLayer(const nlohmann::json& layer_json, Scene* scene)
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
                if (object_json.value("point",false)) {
                    continue;       // TODO:点对象处理方式
                } else if (object_json.value("ellipse",false)) {
                    continue;       // TODO:椭圆对象处理方式
                } else if (object_json.value("polyline",false)) {
                    continue;       // TODO:多边形对象处理方式
                }
                // 没有这些表示则默认时矩形对象
                else {
                    const std::string& object_name = object_json.value("name", "Unnamed");
                    auto game_object = std::make_unique<engine::object::GameObject>(object_name);
                    auto position = glm::vec2(object_json.value("x", 0.0f), object_json.value("y", 0.0f));
                    auto dst_size = glm::vec2(object_json.value("width", 0.0f), object_json.value("height", 0.0f));
                    auto rotation = object_json.value("rotation", 0.0f);
                    game_object->addComponent<engine::component::TransformComponent>(position, glm::vec2(1.0f), rotation);

                    // 添加碰撞组件和物理组件
                    auto collider = std::make_unique<engine::physics::AABBCollider>(dst_size);
                    auto* cc = game_object->addComponent<engine::component::ColliderComponent>(std::move(collider));
                        // 自定义形状对象通常是 trigger 类型的，除非显示指定
                    cc->setTrigger(object_json.value("trgger", true));
                    game_object->addComponent<engine::component::PhysicsComponent>(&scene->getContext().getPhysicsEngine(), false);

                    // 获取标签信息并设置
                    if (auto tag = getTileProperty<std::string>(object_json, "tag"); tag) {
                        game_object->setTag(tag.value());
                    }

                    // 添加到场景中
                    scene->addGameObject(std::move(game_object));
                    spdlog::info("Loaded object: {}", object_name);
                }
            }
            else
            {
                auto tile_info = getTileInfoByGid(gid);
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
                // 如果是陷阱瓦片，且没有手动设置标签，则自动设置标签为 "hazard"
                else if (tile_info.type == engine::component::TileType::HAZARD){
                    game_object->setTag("hazard");
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

                // 获取动画信息并设置
                auto anim_string = getTileProperty<std::string>(tile_json, "animation");
                if (anim_string){
                    nlohmann::json anim_json;
                    try
                    {
                        anim_json = nlohmann::json::parse(anim_string.value());
                    }
                    catch(const nlohmann::json::parse_error& e)
                    {
                        spdlog::error("解析动画 JSON 字符串失败：{}", e.what());
                        continue;   // 跳过当前对象
                    }
                    // 添加 AnimationComponent
                    auto* ac = game_object->addComponent<engine::component::AnimationComponent>();
                    addAnimation(anim_json, ac, src_size);
                }

                // 获取音效信息并设置
                auto sound_string = getTileProperty<std::string>(tile_json, "sound");
                if (sound_string){
                    nlohmann::json sound_json;
                    try
                    {
                        sound_json = nlohmann::json::parse(sound_string.value());
                    }
                    catch(const std::exception& e)
                    {
                        spdlog::error("解析音效 JSON 字符串失败：{}", e.what());
                        continue;   // 跳过当前对象
                    }
                    auto* audio_component = game_object->addComponent<engine::component::AudioComponent>(&scene->getContext().getAudioPlayer(),
                                                                                                    &scene->getContext().getCamera());
                    addSound(sound_json, audio_component);
                }

                // 获取生命值信息并设置
                auto health = getTileProperty<int>(tile_json, "health");
                if (health){
                    game_object->addComponent<engine::component::HealthComponent>(health.value());
                }

                // 添加到场景中
                scene->addGameObject(std::move(game_object));
                spdlog::info("Loaded object: {}", object_name);
            }
        }
    }

    void LevelLoader::addAnimation(const nlohmann::json &anim_json, engine::component::AnimationComponent *ac, const glm::vec2 &sprite_size)
    {
        if (!anim_json.is_object() || !ac) {
            spdlog::error("无效的动画 JSON 对象或 AnimationComponent 指针。");
            return;
        }

        // 遍历动画 JSON 对象中的每个键值对
        for (const auto& anim:anim_json.items()) {
            const std::string& anim_name = anim.key();
            const auto& anim_info = anim.value();
            if (!anim_info.is_object()){
                spdlog::error("动画 '{}' 的信息无效或为空。", anim_name);
                continue;
            }
            // 获取可能存在的动画帧信息
            auto duration_ms = anim_info.value("duration",100);         // 默认为100ms
            auto duration = static_cast<float>(duration_ms) / 1000.0f;  // 转换为秒
            auto row = anim_info.value("row",0);    // 默认行数为0
            // 帧信息（数组）必须存在
            if (!anim_info.contains("frames") || !anim_info["frames"].is_array()) {
                spdlog::warn("动画 '{}' 缺少帧信息。", anim_name);
                continue;
            }
            auto animation = std::make_unique<engine::render::Animation>(anim_name);
            for (const auto& frame : anim_info["frames"]) {
                if (!frame.is_number_integer()){
                    spdlog::warn("动画 {} 中 frames 数组格式错误！", anim_name);
                    continue;
                }
                auto column = frame.get<int>();
                SDL_FRect src_rect = {
                    column * sprite_size.x,
                    row * sprite_size.y,
                    sprite_size.x,
                    sprite_size.y
                };
                animation->addFrame(src_rect, duration);
            }
            ac->addAnimation(std::move(animation));
        }
    }

void LevelLoader::addSound(const nlohmann::json & sound_json, engine::component::AudioComponent * audio_component)
{
    if (!sound_json.is_object() || !audio_component) {
        spdlog::error("无效的音频 JSON 对象或 AudioComponent 指针。");
        return;
    }
    for (const auto& sound : sound_json.items()) {
        const std::string& sound_id = sound.key();
        const std::string& sound_path = sound.value();
        if (sound_id.empty() || sound_path.empty()) {
            spdlog::warn("音效 '{}' 缺少必要信息", sound_id);
            continue;
        }
        audio_component->addSound(sound_id, sound_path);
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
                else if (property.contains("name") && property["name"] == "unisolid") {
                    bool is_unisolid = property.value("value", false);
                    return is_unisolid ? engine::component::TileType::UNISOLID : engine::component::TileType::NORMAL;

                }
                else if (property.contains("name") && property["name"] == "slope") {
                    auto slope_type = property.value("value", "");
                    if (slope_type == "0_1") {
                        return engine::component::TileType::SLOPE_0_1;
                    } else if (slope_type == "1_0") {
                        return engine::component::TileType::SLOPE_1_0;
                    } else if (slope_type == "0_2") {
                        return engine::component::TileType::SLOPE_0_2;
                    } else if (slope_type == "2_0") {
                        return engine::component::TileType::SLOPE_2_0;
                    } else if (slope_type == "2_1") {
                        return engine::component::TileType::SLOPE_2_1;
                    } else if (slope_type == "1_2") {
                        return engine::component::TileType::SLOPE_1_2;
                    } else {
                        spdlog::error("Unknown slope type: {}", slope_type);
                        return engine::component::TileType::NORMAL;
                    }
                }
                else if (property.contains("name") && property["name"] == "hazard") {
                    auto is_hazard = property.value("value", false);
                    return is_hazard ? engine::component::TileType::HAZARD : engine::component::TileType::NORMAL;
                }
                else if (property.contains("name") && property["name"] == "ladder") {
                    auto if_ladder = property.value("value", false);
                    return if_ladder ? engine::component::TileType::LADDER : engine::component::TileType::NORMAL;
                }
                // TODO: 添加更多类型的tile type
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

    engine::component::TileInfo LevelLoader::getTileInfoByGid(int gid)
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