#pragma once

#include "component.h"
#include "../render/sprite.h"
#include <string>
#include <glm/vec2.hpp>

namespace engine::component {
class TransformComponent;

class ParallaxComponent final : public Component {
    friend class engine::object::GameObject;

private:
    TransformComponent* transform_ = nullptr;       // 缓存变换组件

    engine::render::Sprite sprite_;              // 精灵对象
    glm::vec2 scroll_factor_;                // 滚动速度因子（0=静止，1=随相机移动，<1=比相机慢）
    glm::bvec2 repeat_;                      // 是否沿着 x 和 y 轴重复
    bool is_hidden_ = false;               // 是否隐藏（不渲染）

public:
    ParallaxComponent(const std::string& texture_id, const glm::vec2& scroll_factor, const glm::bvec2& repeat);

    // setters
    void setSprite(const engine::render::Sprite& sprite) { sprite_ = sprite; }
    void setScrollFactor(const glm::vec2& scroll_factor) { scroll_factor_ = scroll_factor; }
    void setRepeat(const glm::bvec2& repeat) { repeat_ = repeat; }
    void setHidden(bool is_hidden) { is_hidden_ = is_hidden; }

    // getters
    const engine::render::Sprite& getSprite() const { return sprite_; }
    const glm::vec2& getScrollFactor() const { return scroll_factor_; }
    const glm::bvec2& getRepeat() const { return repeat_; }
    bool isHidden() const { return is_hidden_; }

protected:
    void update(float, engine::core::Context&) override {}
    void init() override;
    void render(engine::core::Context&) override;

};

}   // namespace engine::component
