#pragma once

#include "ui_element.h"
#include "../render/sprite.h"

namespace engine::ui {

class UIImage final : public UIElement {
protected:
    engine::render::Sprite sprite_;

public:
    UIImage(const std::string& texture_id,
            const glm::vec2& position = {0.0f, 0.0f},
            const glm::vec2& size = {0.0f, 0.0f},
            const std::optional<SDL_FRect>& source_rect = std::nullopt,
            bool is_flipped = false);

    void render(engine::core::Context& context) override;

    // setters and getters
    const engine::render::Sprite& getSprite() const { return sprite_; }
    void setSprite(const engine::render::Sprite& sprite) { sprite_ = sprite; }

    const std::string& getTextureId() const { return sprite_.getTextureId(); }
    void setTextureId(const std::string& texture_id) { sprite_.setTextureId(texture_id); }

    const std::optional<SDL_FRect>& getSourceRect() const { return sprite_.getSourceRect(); }
    void setSourceRect(const std::optional<SDL_FRect>& source_rect) { sprite_.setSourceRect(source_rect); }

    bool isFlipped() const { return sprite_.isFlipped(); }
    void setFlipped(bool is_flipped) { sprite_.setFlipped(is_flipped); }
};

} // namespace engine::ui