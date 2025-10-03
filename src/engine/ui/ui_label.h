#pragma once

#include "ui_element.h"

namespace engine::render {
    class TextRenderer;
}

namespace engine::ui {

class UILabel final : public UIElement {
private:
    engine::render::TextRenderer& text_renderer_;   // 用于获取/更新文本的尺寸

    std::string text_;
    std::string font_id_;
    int font_size_;
    engine::utils::FColor text_fcolor_ = {1.0f, 1.0f, 1.0f, 1.0f};

public:
    UILabel(engine::render::TextRenderer& text_renderer,
            const std::string& text,
            const std::string& font_id,
            int font_size = 16,
            const engine::utils::FColor& text_color = {1.0f, 1.0f, 1.0f, 1.0f},
            const glm::vec2& position = {0.0f, 0.0f});

    void render(engine::core::Context& context) override;

    // setters and getters
    const std::string& getText() const { return text_; }
    const std::string& getFontId() const { return font_id_; }
    int getFontSize() const { return font_size_; }
    const engine::utils::FColor& getTextFColor() const { return text_fcolor_; }

    void setText(const std::string& text);
    void setFontId(const std::string& font_id);
    void setFontSize(int font_size);
    void setTextFColor(const engine::utils::FColor& text_color);
};


}   // namespace engine::ui