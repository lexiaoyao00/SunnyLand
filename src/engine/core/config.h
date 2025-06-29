#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <nlohmann/json_fwd.hpp>

namespace engine::core {

class Config final{
public:
    std::string window_title_ = "SunnyLand";
    int window_width_ = 1280;
    int window_height_ = 720;
    bool window_resizable_ = true;

    bool vsync_enabled_ = true;
    int target_fps_ = 144;

    float music_volume_ = 0.5f;
    float sound_volume_ = 0.5f;

    std::unordered_map<std::string, std::vector<std::string>> input_mappings_ = {
        {"move_up", {"W", "UP"}},
        {"move_down", {"S", "DOWN"}},
        {"move_left", {"A", "LEFT"}},
        {"move_right", {"D", "RIGHT"}},
        {"jump", {"J", "SPACE"}},
        {"attack", {"F", "MouseLeft"}},
        {"pause", {"P", "Escape"}},

        // more...
    };

    explicit Config(const std::string& config_path);

    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) = delete;
    Config& operator=(Config&&) = delete;

    bool loadFromFile(const std::string& config_path);
    [[nodiscard]] bool saveToFile(const std::string& config_path);

private:
    void formJson(const nlohmann::json& json);      // 从 Json 对象反序列化配置
    nlohmann::ordered_json toJson() const;          // 将配置序列化为 Json 对象
};

} // namespace engine::core