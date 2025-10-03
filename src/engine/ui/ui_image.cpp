#include "ui_image.h"
#include "../core/context.h"
#include "../render/renderer.h"
#include <spdlog/spdlog.h>


namespace engine::ui {
UIImage::UIImage(const std::string &texture_id,
                 const glm::vec2 &position,
                 const glm::vec2 &size,
                 const std::optional<SDL_FRect> &source_rect,
                 bool is_flipped)
    : UIElement(position, size),
      sprite_(texture_id, source_rect, is_flipped)
{
    if (texture_id.empty()) {
        spdlog::warn("创建了一个空纹理 ID 的 UIImage");
    }
    spdlog::trace("创建了一个 UIImage，纹理 ID 为 {}", texture_id);
}

void UIImage::render(engine::core::Context &context)
{
    if (!visible_ || sprite_.getTextureId().empty()) {
        return;
    }

    auto position = getScreenPosition();
    if (size_.x == 0.0f && size_.y == 0.0f) {   // 尺寸为0使用纹理的原始尺寸
        context.getRenderer().drawUISprite(sprite_, position);
    } else {
        context.getRenderer().drawUISprite(sprite_, position, size_);
    }

    UIElement::render(context);
}


} // namespace engine::ui