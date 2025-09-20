#include "camera.h"
#include <spdlog/spdlog.h>
#include "../component/transform_component.h"

namespace engine::render {

Camera::Camera(const glm::vec2 &viewport_size, const glm::vec2 &position, const std::optional<engine::utils::Rect> viewport_rect)
    : viewport_size_(viewport_size), position_(position), limit_bounds_(viewport_rect)
{
    spdlog::trace("Camera created, position: {},{}", position.x, position.y);
}

void Camera::update(float delta_time)
{
    if (target_ == nullptr) return;

    glm::vec2 target_pos = target_->getPosition();
    glm::vec2 desired_position = target_pos - viewport_size_ / 2.0f;    // 屏幕中心对齐

    auto distance = glm::distance(position_, desired_position);
    constexpr float SNAP_THRESHOLD = 1.0f;  // 设置一个距离阈值（constexpr：编译时常量，避免每次调用计算）

    if (distance < SNAP_THRESHOLD)
    {
        // 距离小于阈值直接吸附到目标位置
        position_ = desired_position;
    }
    else
    {
        // 线性插值平滑移动  glm::mix(a,b,t): 在向量 a, b 之间进行插值，t 是插值因子，范围在0到1之间，
        // 公式：(b-a)*t + a; t = 0 时结果为 a，t = 1 时结果为 b
        position_ = glm::mix(position_, desired_position, smooth_speed_ * delta_time);
        position_ = glm::vec2(glm::round(position_.x), glm::round(position_.y));    // 四舍五入，省略的话可能会出现画面割裂
    }

    clampPosition();

}

void Camera::move(const glm::vec2 &offset)
{
    position_ += offset;
    clampPosition();
}


glm::vec2 Camera::worldToScreen(const glm::vec2 &world_pos) const
{
    // 世界坐标减去相机左上角坐标
    return world_pos - position_;
}

glm::vec2 Camera::worldToScreenWithParallax(const glm::vec2 &world_pos, const glm::vec2 &scroll_factor) const
{
    return world_pos - position_ * scroll_factor;
}

glm::vec2 Camera::screenToWorld(const glm::vec2 &screen_pos) const
{
    return screen_pos + position_;
}

void Camera::setPosition(const glm::vec2 &position)
{
    position_ = position;
    clampPosition();
}

void Camera::setLimitBounds(const std::optional<engine::utils::Rect> &limit_bounds)
{
    limit_bounds_ = limit_bounds;
}

void Camera::setTarget(engine::component::TransformComponent *target)
{
    target_ = target;
}

const glm::vec2 &Camera::getPosition() const
{
    return position_;
}

glm::vec2 Camera::getViewportSize() const
{
    return viewport_size_;
}

std::optional<engine::utils::Rect> Camera::getLimitBounds() const
{
    return limit_bounds_;
}

engine::component::TransformComponent *Camera::getTarget() const
{
    return target_;
}

void Camera::clampPosition()
{
    if (limit_bounds_.has_value() && limit_bounds_->size.x > 0 && limit_bounds_->size.y > 0)
    {
        glm::vec2 min_cam_pos = limit_bounds_->position;
        glm::vec2 max_cam_pos = limit_bounds_->position + limit_bounds_->size - viewport_size_;

        // 确保 max_cam_pos 不小于 min_cam_pos (屏幕视窗可能比世界大)
        max_cam_pos.x = glm::max(max_cam_pos.x, min_cam_pos.x);
        max_cam_pos.y = glm::max(max_cam_pos.y, min_cam_pos.y);

        position_ = glm::clamp(position_, min_cam_pos, max_cam_pos);
    }
}

}   // namespace engine::render