#pragma once
#include "../utils/math.h"

namespace engine::component {
    class ColliderComponent;
}

namespace engine::physics::collision {

bool checkCollision(component::ColliderComponent& a, component::ColliderComponent& b);

bool checkCircleOverlap(const glm::vec2& a_center, float a_radius, const glm::vec2& b_center, float b_radius);

bool checkAABBOverlap(const glm::vec2& a_pos, const glm::vec2& a_size, const glm::vec2& b_pos, const glm::vec2& b_size);

bool checkRectOverlap(const engine::utils::Rect& a, const engine::utils::Rect& b);

bool checkPointInCircle(const glm::vec2& point, const glm::vec2& center, float radius);

// more...

}   // namespace engine::physics::collision