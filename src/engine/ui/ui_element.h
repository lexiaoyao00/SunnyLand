#pragma once
#include <SDL3/SDL_rect.h>
#include <memory>
#include "../utils/math.h"


namespace engine::core {
    class Context;
}


namespace engine::ui {

class UIElement {

protected:
    glm::vec2 position_;        // 相对于父元素的局部位置
    glm::vec2 size_;            // 元素大小
    bool visible_ = true;       // 是否可见
    bool need_remove_ = false;  // 是否需要移除(延迟删除)

    UIElement* parent_ = nullptr;  // 指向父节点的非拥有指针
    std::vector<std::unique_ptr<UIElement>> children_;  // 子元素列表

public:
    explicit UIElement(const glm::vec2& position = {0.0f, 0.0f}, const glm::vec2& size = {0.0f, 0.0f});
    virtual ~UIElement() = default;

    UIElement(const UIElement&) = delete;
    UIElement& operator=(const UIElement&) = delete;
    UIElement(UIElement&&) = delete;
    UIElement& operator=(UIElement&&) = delete;

    virtual bool handleInput(engine::core::Context& context);
    virtual void update(float delta_time, engine::core::Context& context);
    virtual void render(engine::core::Context& context);

    // 层次管理
    void addChild(std::unique_ptr<UIElement> child);            // 添加子元素
    std::unique_ptr<UIElement> removeChild(UIElement* child);   // 从列表中移除指定子元素，并返回其智能指针
    void removeAllChildren();                                   // 移除所有子元素

    // getters and setters
    glm::vec2 getPosition() const { return position_; }
    glm::vec2 getSize() const { return size_; }
    bool isVisible() const { return visible_; }
    bool isNeedRemove() const { return need_remove_; }
    UIElement* getParent() const { return parent_; }
    const std::vector<std::unique_ptr<UIElement>>& getChildren() const { return children_; }

    void setPosition(const glm::vec2& position) { position_ = position; }
    void setSize(const glm::vec2& size) { size_ = size; }
    void setVisible(bool visible) { visible_ = visible; }
    void setNeedRemove(bool need_remove) { need_remove_ = need_remove; }
    void setParent(UIElement* parent) { parent_ = parent; }

    // ------ 辅助方法 ------
    engine::utils::Rect getBounds() const;              // 获取（计算）元素的边界（屏幕坐标）
    glm::vec2 getScreenPosition() const;            // 获取元素的屏幕坐标
    bool isPointInside(const glm::vec2& point) const;  // 判断点是否在元素内
};

}   // namespace engine::ui