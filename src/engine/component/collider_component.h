#pragma once

#include "component.h"
#include "../physics/collider.h"
#include "../utils/math.h"
#include "../utils/alignment.h"
#include <memory>

namespace engine::component {
class TransformComponent;

class ColliderComponent final : public Component {
    friend class engine::object::GameObject;

private:
    TransformComponent* transform_ = nullptr;

    std::unique_ptr<engine::physics::Collider> collider_;   // 拥有的碰撞器对象
    glm::vec2 offset_ = {0.0f, 0.0f};           // 碰撞器（最小包围盒）左上角相对于变换原点的偏移量
    engine::utils::Alignment alignment_ = engine::utils::Alignment::NONE;   // 对齐方式

    bool is_trigger_ = false;   // 是否是触发器(仅检测碰撞，不产生物理响应)
    bool is_active_ = true;   // 是否激活

public:
    explicit ColliderComponent(
        std::unique_ptr<engine::physics::Collider> collider,
        engine::utils::Alignment alignment = engine::utils::Alignment::NONE,
        bool is_trigger = false,
        bool is_active = true
    );

    /**
     * @brief 根据当前的 alignment_anchor_ 和 collider_ 尺寸计算 offset_
     * @note 需要用到 TransformComponent 的 scale, 因此 TransformComponent 更新scale 时需要调用此函数
     */
    void updateOffset();

    // getters
    const TransformComponent* getTransform() const { return transform_; }
    const engine::physics::Collider* getCollider() const { return collider_.get(); }
    const glm::vec2& getOffset() const { return offset_; }
    engine::utils::Alignment getAlignment() const { return alignment_; }
    engine::utils::Rect getWorldAABB() const;           // 获取世界坐标下的最小包围盒
    bool isTrigger() const { return is_trigger_; }
    bool isActive() const { return is_active_; }

    // setters
    void setAlignment(engine::utils::Alignment alignment); // 设置对齐方式，重新计算偏移量
    void setOffset(const glm::vec2& offset) { offset_ = offset; } // 设置偏移量
    void setTrigger(bool is_trigger) { is_trigger_ = is_trigger; }
    void setActive(bool is_active) { is_active_ = is_active; }

private:
    void init() override;
    void update(float, engine::core::Context&) override {}



};

} // namespace engine::component