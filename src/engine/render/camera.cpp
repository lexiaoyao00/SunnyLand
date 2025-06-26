#include "camera.h"
#include <spdlog/spdlog.h>

namespace engine::render {

Camera::Camera(const glm::vec2 &viewport_size, const glm::vec2 &position, const std::optional<engine::utils::Rect> viewport_rect)
    : viewport_size_(viewport_size), position_(position), limit_bounds_(viewport_rect)
{
    spdlog::trace("Camera created, position: {},{}", position.x, position.y);
}

void Camera::update(float /* delta_time */)
{
    // TODO:自动跟随目标
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