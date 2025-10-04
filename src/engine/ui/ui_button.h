#pragma once

#include "ui_interactive.h"

namespace engine::ui {
class UIButton final : public UIInteractive {
private:
    std::function<void()> callback_;        // 可自定义的函数（函数包装器）

public:
    UIButton(engine::core::Context& context,
             const std::string& normal_sprite_id,
             const std::string& hover_sprite_id,
             const std::string& pressed_sprite_id,
             const glm::vec2& position = {0.0f, 0.0f},
             const glm::vec2& size = {0.0f, 0.0f},
             std::function<void()> callback = nullptr);

    ~UIButton() override = default;

    void clicked() override;

    void setCallback(std::function<void()> callback) { callback_ = std::move(callback); }
    std::function<void()> getCallback() const { return callback_; }
};
}   // namespace engine::ui