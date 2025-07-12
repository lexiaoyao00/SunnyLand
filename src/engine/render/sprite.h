#pragma once

#include <SDL3/SDL_rect.h>
#include <string>
#include <optional>

namespace engine::render {

class Sprite final {
private:
    std::string texture_id_;
    std::optional<SDL_FRect> source_rect_;
    bool is_flipped_ = false;

public:
    Sprite() = default;
    Sprite(const std::string& texture_id, const std::optional<SDL_FRect>& source_rect = std::nullopt, bool is_flipped = false)
        :   texture_id_(texture_id),
            source_rect_(source_rect),
            is_flipped_(is_flipped)
    {}

    // getters and setters
    const std::string& getTextureId() const {return texture_id_;}
    const std::optional<SDL_FRect>& getSourceRect() const {return source_rect_;}
    bool isFlipped() const {return is_flipped_;}

    void setTextureId(const std::string& texture_id) {texture_id_ = texture_id;}
    void setSourceRect(const std::optional<SDL_FRect>& source_rect) {source_rect_ = source_rect;}
    void setFlipped(bool is_flipped) {is_flipped_ = is_flipped;}

};



}   // namespace engine::render