#pragma once

#include "ui_element.h"
#include "state/ui_state.h"
#include "../render/sprite.h"       // map容器创建时可能会检测内部元素是否有析构定义

namespace engine::core {
    class Context;
}

namespace engine::ui {

class UIInteractive : public UIElement {

protected:
    engine::core::Context& context_;        // 可交互元素很可能需要其他组件的接口
    std::unique_ptr<engine::ui::state::UIState> state_;    // 当前状态
    std::unordered_map<std::string, std::unique_ptr<engine::render::Sprite>> sprite_;   // 精灵集合
    std::unordered_map<std::string, std::string> sounds_;   // 音效集合
    engine::render::Sprite* current_sprite_ = nullptr;    // 当前显示的精灵
    bool interactive_ = true;   // 是否可交互

public:
    UIInteractive(engine::core::Context& context, const glm::vec2& position = {0.0f, 0.0f}, const glm::vec2& size = {0.0f, 0.0f});
    ~UIInteractive() override;

    virtual void clicked() {}

    void addSprite(const std::string& name, std::unique_ptr<engine::render::Sprite> sprite);
    void setSprite(const std::string& name);
    void addSound(const std::string& name, const std::string& path);
    void playSound(const std::string& name);

    void setState(std::unique_ptr<engine::ui::state::UIState> state);
    engine::ui::state::UIState* getState() const { return state_.get(); }

    void setInteractive(bool interactive) { interactive_ = interactive; }
    bool isInteractive() const { return interactive_; }

    bool handleInput(engine::core::Context& context) override;
    void render(engine::core::Context& context) override;


};


}   // namespace engine::ui