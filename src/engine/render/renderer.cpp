#include "renderer.h"
#include "camera.h"
#include "../resource/resource_manager.h"
#include <spdlog/spdlog.h>
#include <SDL3_image/SDL_image.h>

namespace engine::render {


Renderer::Renderer(SDL_Renderer *renderer, engine::resource::ResourceManager *resource_manager)
    :   renderer_(renderer), resource_manager_(resource_manager)
{
    spdlog::trace("creating Renderer ...");
    if (renderer_ == nullptr) {
        throw std::runtime_error("Renderer created fail, SDL_Renderer is null");
    }

    if (resource_manager_ == nullptr) {
        throw std::runtime_error("Renderer created fail, ResourceManager is null");
    }

    setDrawColor(0, 0, 0, 255);
    spdlog::trace("Renderer created");
}

void Renderer::drawSprite(const Camera &camera, const Sprite &sprite, const glm::vec2 &position, const glm::vec2 &scale, double angle)
{
    auto texture = resource_manager_->getTexture(sprite.getTextureId());
    if (texture == nullptr) {
        spdlog::error("drawSprite fail, texture is null, ID: {}", sprite.getTextureId());
        return;
    }

    auto src_rect = getSpriteSrcRect(sprite);
    if (!src_rect.has_value()) {
        spdlog::error("drawSprite fail, src_rect is null, ID: {}", sprite.getTextureId());
        return;
    }

    // 应用相机变换
    glm::vec2 position_screen = camera.worldToScreen(position);

    // 计算目标矩形
    float scaled_w = src_rect.value().w * scale.x;
    float scaled_h = src_rect.value().h * scale.y;
    SDL_FRect dst_rect = {position_screen.x, position_screen.y, scaled_w, scaled_h};

    // 如果目标矩形不在视口内,则不绘制
    if (!isRectInViewport(camera, dst_rect)) return;

    if (!SDL_RenderTextureRotated(renderer_, texture, &src_rect.value(), &dst_rect, angle, nullptr, sprite.getIsFlipped() ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE)) {
        spdlog::error("drawSprite fail, SDL_RenderTexture fail, ID: {}", sprite.getTextureId());
    }

}

void Renderer::drawParallax(const Camera &camera, const Sprite &sprite, const glm::vec2 &position, const glm::vec2 &scroll_factor, const glm::bvec2 &repeat, const glm::vec2 &scale)
{
    auto texture = resource_manager_->getTexture(sprite.getTextureId());
    if (texture == nullptr) {
        spdlog::error("drawParallax fail, texture is null, ID: {}", sprite.getTextureId());
        return;
    }

    auto src_rect = getSpriteSrcRect(sprite);
    if (!src_rect.has_value()) {
        spdlog::error("drawParallax fail, src_rect is null, ID: {}", sprite.getTextureId());
        return;
    }

    // 应用相机变换
    glm::vec2 position_screen = camera.worldToScreenWithParallax(position, scroll_factor);

    // 计算目标矩形
    float scaled_w = src_rect.value().w * scale.x;
    float scaled_h = src_rect.value().h * scale.y;

    glm::vec2 start,stop;
    glm::vec2 viewport_size = camera.getViewportSize();

    if (repeat.x) {
        start.x = glm::mod(position_screen.x, scaled_w) - scaled_w;
        stop.x = viewport_size.x;
    } else {
        start.x = position_screen.x;
        stop.x = glm::min(position_screen.x + scaled_w, viewport_size.x);
    }

    if (repeat.y) {
        start.y = glm::mod(position_screen.y, scaled_h) - scaled_h;
        stop.y = viewport_size.y;
    } else {
        start.y = position_screen.y;
        stop.y = glm::min(position_screen.y + scaled_h, viewport_size.y);
    }

    for (float y = start.y; y < stop.y; y += scaled_h) {
        for (float x = start.x; x < stop.x; x += scaled_w) {
            SDL_FRect dst_rect = {x, y, scaled_w, scaled_h};
            if (!SDL_RenderTexture(renderer_, texture, nullptr, &dst_rect)) {
                spdlog::error("drawParallax fail, SDL_RenderTexture fail, ID: {}", sprite.getTextureId());
                return;
            }
        }
    }

}

void Renderer::drawUISprite(const Sprite &sprite, const glm::vec2 &postion, const std::optional<glm::vec2> &size)
{
    auto texture = resource_manager_->getTexture(sprite.getTextureId());
    if (texture == nullptr) {
        spdlog::error("drawUISprite fail, texture is null, ID: {}", sprite.getTextureId());
        return;
    }

    auto src_rect = getSpriteSrcRect(sprite);
    if (!src_rect.has_value()) {
        spdlog::error("drawUISprite fail, src_rect is null, ID: {}", sprite.getTextureId());
        return;
    }

    SDL_FRect dst_rect = {postion.x, postion.y, 0, 0};
    if (size.has_value()) {
        dst_rect.w = size.value().x;
        dst_rect.h = size.value().y;
    } else {
        dst_rect.w = src_rect.value().w;
        dst_rect.h = src_rect.value().h;
    }

    if (!SDL_RenderTextureRotated(renderer_, texture, &src_rect.value(), &dst_rect,0.0, nullptr, sprite.getIsFlipped() ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE)) {
        spdlog::error("drawUISprite fail, SDL_RenderTexture fail, ID: {}", sprite.getTextureId());
    }
}

void Renderer::present()
{
    SDL_RenderPresent(renderer_);
}

void Renderer::clearScreen()
{
    SDL_RenderClear(renderer_);
}

void Renderer::setDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    if (!SDL_SetRenderDrawColor(renderer_, r, g, b, a)) {
        spdlog::error("setDrawColor fail, SDL_SetRenderDrawColor fail :{}", SDL_GetError());
    }
}

void Renderer::setDrawColorFloat(float r, float g, float b, float a)
{
    if (!SDL_SetRenderDrawColorFloat(renderer_, r, g, b, a)) {
        spdlog::error("setDrawColorFloat fail, SDL_SetRenderDrawColorFloat fail :{}", SDL_GetError());
    }
}

std::optional<SDL_FRect> Renderer::getSpriteSrcRect(const Sprite &sprite)
{
    SDL_Texture *texture = resource_manager_->getTexture(sprite.getTextureId());
    if (texture == nullptr) {
        spdlog::error("getSpriteSrcRect fail, texture is null");
        return std::nullopt;
    }
    auto src_rect = sprite.getSourceRect();
    if (src_rect.has_value()){
        if (src_rect.value().w <= 0 || src_rect.value().h <= 0) {
            spdlog::error("getSpriteSrcRect fail, src_rect is invalid,ID: {} ",sprite.getTextureId());
            return std::nullopt;
        }
        return src_rect;
    } else {
        SDL_FRect result = {0, 0, 0, 0};
        if (!SDL_GetTextureSize(texture, &result.w, &result.h)) {
            spdlog::error("getSpriteSrcRect fail, get texture size fail,ID: {} ",sprite.getTextureId());
            return std::nullopt;
        }

        return result;
    }

    return std::optional<SDL_FRect>();
}

bool Renderer::isRectInViewport(const Camera &camera, const SDL_FRect &rect)
{
    glm::vec2 viewport_size = camera.getViewportSize();
    return (rect.x + rect.w >= 0 && rect.x <= viewport_size.x &&
            rect.y + rect.h >= 0 && rect.y <= viewport_size.y);
}

} // namespace engine::render