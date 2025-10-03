#pragma once
#include <memory>
#include <glm/vec2.hpp>


namespace engine::core {
    class Context;
}

namespace engine::ui {
    class UIElement;
    class UIPanel;  // UIPanel 作为根元素

class UIManager final {

private:
    std::unique_ptr<UIPanel> root_element_;  // 根元素

public:
    UIManager();
    ~UIManager();

    UIManager(const UIManager&) = delete;
    UIManager& operator=(const UIManager&) = delete;
    UIManager(UIManager&&) = delete;
    UIManager& operator=(UIManager&&) = delete;

    [[nodiscard]] bool init(const glm::vec2& window_size);  // 初始化 UIManager，设置根元素的大小
    void addElement(std::unique_ptr<UIElement> element);
    UIPanel* getRootElement() const;
    void clearElements();

    // 核心循环方法
    bool handleInput(engine::core::Context&);
    void update(float delta_time, engine::core::Context&);
    void render(engine::core::Context&);
};

}   // namespace engine::ui