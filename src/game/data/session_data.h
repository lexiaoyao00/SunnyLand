#pragma once
#include <string>
#include <memory>
#include <nlohmann/json.hpp>


namespace game::data {

class SessionData final {
private:
    int current_health_ = 3;
    int max_health_ = 3;
    int current_score_ = 0;
    int high_score_ = 0;

    int level_health_ = 3;  // 进入关卡时的生命值(读/存档用)
    int level_score_ = 0;  // 进入关卡时的得分(读/存档用)
    std::string map_path_ = "assets/maps/level1.tmj";

public:
    SessionData() = default;
    ~SessionData() = default;

    SessionData(const SessionData&) = delete;
    SessionData& operator=(const SessionData&) = delete;
    SessionData(SessionData&&) = delete;
    SessionData& operator=(SessionData&&) = delete;

    // Getters
    int getCurrentHealth() const { return current_health_; }
    int getMaxHealth() const { return max_health_; }
    int getCurrentScore() const { return current_score_; }
    int getHighScore() const { return high_score_; }
    int getLevelHealth() const { return level_health_; }
    int getLevelScore() const { return level_score_; }
    const std::string& getMapPath() const { return map_path_; }

    // Setters
    void setCurrentHealth(int current_health);
    void setMaxHealth(int max_health);
    void addScore(int score_to_add);
    void setHighScore(int high_score) { high_score_ = high_score; }
    void setLevelHealth(int level_health) { level_health_ = level_health; }
    void setLevelScore(int level_score) { level_score_ = level_score; }
    void setMapPath(const std::string& map_path) { map_path_ = map_path; }

    void reset();                   // 重置游戏数据以准备开始新游戏（保留最高分）
    void setNextLevel(const std::string& map_path);  // 设置下一关场景信息（地图，关卡开始时的得分，生命值）
    bool saveToFile(const std::string& file_path) const;  // 保存当前游戏数据到json文件
    bool loadFromFile(const std::string& file_path);  // 从json文件加载游戏数据

};

}   // namespace game::data