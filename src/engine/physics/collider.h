#pragma once
#include <glm/vec2.hpp>

namespace engine::physics {
enum class ColliderType {
    NONE,
    AABB,
    CIRCLE,
    // MORE...
};

class Collider {
protected:
    glm::vec2 aabb_size_ = {0.0f, 0.0f};         // 覆盖 Collier 的最小包围盒的尺寸（宽度和高度），某些情况下可以简化计算

public:
    virtual ~Collider() = default;
    virtual ColliderType getType() const = 0;

    void setAABBSize(const glm::vec2& size) { aabb_size_ = size; }
    const glm::vec2& getAABBSize() const { return aabb_size_; }

};

/**
 * @brief 轴对齐包围盒 Axis-Aligned Bounding Box Collider
 *
 */
class AABBCollider final : public Collider {
private:
    glm::vec2 size_ = {0.0f, 0.0f};     // 包围盒的尺寸，与 aabb_size_ 相同

public:
    explicit AABBCollider(const glm::vec2& size) : size_(size) { setAABBSize(size); }
    ~AABBCollider() override = default;

    ColliderType getType() const override { return ColliderType::AABB; }
    const glm::vec2& getSize() const { return size_; }
    void setSize(const glm::vec2& size) { size_ = size; }

};

class CircleCollider final : public Collider {
private:
    float radius_ = 0.0f;     // 圆的半径

public:
    explicit CircleCollider(float radius) : radius_(radius) { setAABBSize({radius * 2.0f, radius * 2.0f}); }
    ~CircleCollider() override = default;

    ColliderType getType() const override { return ColliderType::CIRCLE; }
    float getRadius() const { return radius_; }
    void setRadius(float radius) { radius_ = radius; }
};

}   // namespace engine::physics