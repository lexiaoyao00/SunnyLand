#include "config.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

namespace engine::core {

Config::Config(const std::string &config_path)
{
    loadFromFile(config_path);
}

bool Config::loadFromFile(const std::string &config_path)
{
    std::ifstream file(config_path);
    if (!file.is_open()) {
        spdlog::warn("Failed to open config file: {},use default config", config_path);
        if (!saveToFile(config_path)) {
            spdlog::error("Failed to save default config file: {}", config_path);
            return false;
        }
        return false;
    }

    try
    {
        nlohmann::json json;
        file >> json;
        formJson(json);
        spdlog::info("Load config file success: {}", config_path);
        return true;
    }
    catch(const std::exception& e)
    {
        spdlog::error("Failed to load config file: {}, error: {}, use default config", config_path, e.what());
    }

    return false;
}

bool Config::saveToFile(const std::string &config_path)
{
    std::ofstream file(config_path);
    if (!file.is_open()) {
        spdlog::error("Failed to open config file '{}' to save config", config_path);
        return false;
    }
    try
    {
        nlohmann::ordered_json json = toJson();
        file << json.dump(4);
        spdlog::info("Save config file success: {}", config_path);
        return true;
    }
    catch(const std::exception& e)
    {
        spdlog::error("Failed to save config file: {}, error: {}", config_path, e.what());
    }
    return false;
}

void Config::formJson(const nlohmann::json &json)
{
    if (json.contains("window")){
        const auto& window_config = json["window"];
        window_title_ = window_config.value("title", window_title_);
        window_width_ = window_config.value("width", window_width_);
        window_height_ = window_config.value("height", window_height_);
        window_resizable_ = window_config.value("resizable", window_resizable_);
    }

    if (json.contains("graphics")) {
        const auto& graphics_config = json["graphics"];
        vsync_enabled_ = graphics_config.value("vsync", vsync_enabled_);
    }

    if (json.contains("performance")){
        const auto& graphics_config = json["performance"];
        target_fps_ = graphics_config.value("target_fps", target_fps_);
        if (target_fps_ < 0) {
            spdlog::warn("target_fps is less than 0, set to 0");
            target_fps_ = 0;
        }
    }

    if (json.contains("audio")){
        const auto& audio_config = json["audio"];
        music_volume_ = audio_config.value("music_volume", music_volume_);
        sound_volume_ = audio_config.value("sound_volume", sound_volume_);
    }

    if (json.contains("input_mappings") && json["input_mappings"].is_object()){
        const auto& mappings_json = json["input_mappings"];
        try
        {
            auto input_mappings = mappings_json.get<std::unordered_map<std::string,std::vector<std::string>>>();
            input_mappings_ = std::move(input_mappings);
            spdlog::trace("Load input mappings success from config file.");
        }
        catch(const std::exception& e)
        {
            spdlog::warn("Failed to load input mappings from config file. Use default config. Error: {}", e.what());
        }

    } else {
        spdlog::trace("No 'input_mappings' in config file or not an object. Use default config.");
    }

}

nlohmann::ordered_json Config::toJson() const
{
    return nlohmann::ordered_json{
        {"window", {
            {"title", window_title_},
            {"width", window_width_},
            {"height", window_height_},
            {"resizable", window_resizable_}
        }},
        {"graphics", {
            {"vsync", vsync_enabled_}
        }},
        {"performance", {
            {"target_fps", target_fps_}
        }},
        {"audio", {
            {"music_volume", music_volume_},
            {"sound_volume", sound_volume_}
        }},
        {"input_mappings", input_mappings_}
    };
}

} // namespace engine::core
