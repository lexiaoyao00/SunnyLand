#include "session_data.h"

#include <spdlog/spdlog.h>
#include <glm/common.hpp>
#include <fstream>

namespace game::data {
void SessionData::setCurrentHealth(int current_health)
{
    current_health_ = glm::clamp(current_health, 0, max_health_);
}

void SessionData::setMaxHealth(int max_health)
{
    if (max_health > 0) {
        max_health_ = max_health;
        setCurrentHealth(current_health_);
    } else {
        spdlog::warn("尝试将最大生命值设置为非正数: {}", max_health);
    }
}

void SessionData::addScore(int score_to_add)
{
    current_score_ += score_to_add;
    setHighScore(glm::max(current_score_, high_score_));
}

void SessionData::reset()
{
    current_health_ = max_health_;
    current_score_ = 0;

    level_health_ = 3;  // 进入关卡时的生命值(读/存档用)
    level_score_ = 0;  // 进入关卡时的得分(读/存档用)
    map_path_ = "assets/maps/level1.tmj";
    spdlog::info("重置游戏数据");
}

void SessionData::setNextLevel(const std::string &map_path)
{
    map_path_ = map_path;
    level_health_ = current_health_;
    level_score_ = current_score_;
}

bool SessionData::saveToFile(const std::string &file_path) const
{
    nlohmann::json j;
    try
    {
        j["level_score"] = level_score_;
        j["level_health"] = level_health_;
        j["max_health"] = max_health_;
        j["high_score"] = high_score_;
        j["map_path"] = map_path_;


        std::ofstream ofs(file_path);
        if (!ofs.is_open())
        {
            spdlog::error("无法打开存档文件进行写入：{}", file_path);
            return false;
        }

        ofs << j.dump(4);
        ofs.close();

        spdlog::info("存档成功：{}", file_path);
        return true;
    }
    catch(const std::exception& e)
    {
        spdlog::error("存档失败：{}，错误信息：{}", file_path, e.what());
        return false;
    }
}

bool SessionData::loadFromFile(const std::string &file_path)
{
    try
    {
        std::ifstream ifs(file_path);
        if (!ifs.is_open())
        {
            spdlog::error("无法打开存档文件进行读取：{}", file_path);
            return false;
        }

        nlohmann::json j;
        ifs >> j;
        ifs.close();

        current_score_ = level_score_ = j.value("level_score", 0);
        current_health_ = level_health_ = j.value("level_health", 3);
        max_health_ = j.value("max_health", 3);
        high_score_ = j.value("high_score", 0);
        map_path_ = j.value("map_path", "assets/maps/level1.tmj");

        spdlog::info("读档成功：{}", file_path);
        return true;
    } catch (const std::exception& e)
    {
        spdlog::error("读档失败：{}，错误信息：{}", file_path, e.what());
        return false;
    }
}

}   // namespace game::data
