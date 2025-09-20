#pragma once
#include "../utils/math.h"
#include <optional>


namespace engine::component {
    class TransformComponent;
}

namespace engine::render {

class Camera final {
private:
    glm::vec2 viewport_size_;   // 屏幕大小(视窗大小)
    glm::vec2 position_;        // 相机左上角的世界坐标
    std::optional<engine::utils::Rect> limit_bounds_;   // 限制相机移动范围，空值表示不限制
    float smooth_speed_ = 5.0f;  // 相机移动平滑速度
    engine::component::TransformComponent* target_ = nullptr;  // 相机跟随的目标

public:
    Camera(const glm::vec2& viewport_size, const glm::vec2& position = glm::vec2(0.0f, 0.0f), const std::optional<engine::utils::Rect> viewport_rect = std::nullopt);

    void update(float delta_time);
    void move(const glm::vec2& offset);

    glm::vec2 worldToScreen(const glm::vec2& world_pos) const; // 将世界坐标转换为屏幕坐标
    glm::vec2 worldToScreenWithParallax(const glm::vec2& world_pos, const glm::vec2& scroll_factor) const; // 将世界坐标转换为屏幕坐标，考虑视差滚动
    glm::vec2 screenToWorld(const glm::vec2& screen_pos) const; // 将屏幕坐标转换为世界坐标

    void setPosition(const glm::vec2& position);        // 设置相机位置
    void setLimitBounds(const std::optional<engine::utils::Rect>& limit_bounds);   // 设置相机移动范围
    void setTarget(engine::component::TransformComponent* target);   // 设置相机跟随的目标

    const glm::vec2& getPosition() const;   // 获取相机位置
    glm::vec2 getViewportSize() const;   // 获取屏幕大小
    std::optional<engine::utils::Rect> getLimitBounds() const;   // 获取相机移动范围
    engine::component::TransformComponent* getTarget() const;   // 获取相机跟随的目标


    Camera(const Camera&) = delete;
    Camera& operator=(const Camera&) = delete;
    Camera(Camera&&) = delete;
    Camera& operator=(Camera&&) = delete;


private:
    void clampPosition();           // 将相机位置限制在限制范围内
};

}   // namespace engine::render