#include "ui_label.h"

#include "../render/text_renderer.h"
#include <spdlog/spdlog.h>

namespace engine::ui {


UILabel::UILabel(engine::render::TextRenderer &text_renderer,
                 const std::string &text,
                 const std::string &font_id,
                 int font_size,
                 const engine::utils::FColor &text_color,
                 const glm::vec2 &position)
    : UIElement(position),
      text_renderer_(text_renderer),
      text_(text),
      font_id_(font_id),
      font_size_(font_size),
      text_fcolor_(text_color)
{
    size_ = text_renderer_.getTextSize(text_, font_id_, font_size_);
    spdlog::trace("UILabel 构建完成");
}

void UILabel::render(engine::core::Context &context)
{
    if (!visible_ || text_.empty()) return;

    text_renderer_.drawUIText(text_, font_id_, font_size_, getScreenPosition(), text_fcolor_);

    UIElement::render(context);
}

void UILabel::setText(const std::string &text)
{
    text_ = text;
    size_ = text_renderer_.getTextSize(text_, font_id_, font_size_);
}

void UILabel::setFontId(const std::string &font_id)
{
    font_id_ = font_id;
    size_ = text_renderer_.getTextSize(text_, font_id_, font_size_);
}

void UILabel::setFontSize(int font_size)
{
    font_size_ = font_size;
    size_ = text_renderer_.getTextSize(text_, font_id_, font_size_);
}

void UILabel::setTextFColor(const engine::utils::FColor &text_color)
{
    text_fcolor_ = text_color;
}

}   // namespace engine::ui
