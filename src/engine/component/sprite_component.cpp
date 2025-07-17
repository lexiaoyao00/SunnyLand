#include "sprite_component.h"
#include "./transform_component.h"
#include "../resource/resource_manager.h"
#include "../object/game_object.h"
#include "../core/context.h"
#include "../render/renderer.h"
#include <spdlog/spdlog.h>

namespace engine::component {

SpriteComponent::SpriteComponent(
    const std::string &texture_id,
    engine::resource::ResourceManager &resource_manager,
    engine::utils::Alignment alignment,
    std::optional<SDL_FRect> source_rect_opt,
    bool is_flipped
) : resource_manager_(&resource_manager), sprite_(texture_id, source_rect_opt, is_flipped),
    alignment_(alignment)
{
    if (!resource_manager_)
    {
        spdlog::critical("SpriteComponent::SpriteComponent: resource_manager_ is null");
        // 不要在游戏主循环中使用 try...catch / throw, 这样会极大影响性能
    }

    // offset_ 和 sprite_size_ 的初始化在 init 中完成
    spdlog::trace("SpriteComponent::SpriteComponent: created, texture_id = {}", texture_id);
}

SpriteComponent::SpriteComponent(engine::render::Sprite &&sprite, engine::resource::ResourceManager &resource_manager, engine::utils::Alignment alignment)
: resource_manager_(&resource_manager), sprite_(std::move(sprite)), alignment_(alignment)
{
    if (!resource_manager_)
    {
        spdlog::critical("SpriteComponent::SpriteComponent: resource_manager_ is null, this may cause undefined behavior");
    }

    spdlog::trace("SpriteComponent::SpriteComponent: created, texture_id = {}", sprite_.getTextureId());
}

void SpriteComponent::updateOffset()
{
    if (sprite_size_.x <= 0 || sprite_size_.y <= 0)
    {
        offset_ = {0, 0};
        return;
    }

    auto scale = transform_->getScale();

    switch (alignment_)
    {
    case engine::utils::Alignment::TOP_LEFT      : offset_ = glm::vec2{0.0f                    , 0.0f                    } * scale; break;
    case engine::utils::Alignment::TOP_CENTER    : offset_ = glm::vec2{-  sprite_size_.x / 2.0f, 0.0f                    } * scale; break;
    case engine::utils::Alignment::TOP_RIGHT     : offset_ = glm::vec2{-  sprite_size_.x       , 0.0f                    } * scale; break;
    case engine::utils::Alignment::CENTER_LEFT   : offset_ = glm::vec2{0.0f                    , -  sprite_size_.y / 2.0f} * scale; break;
    case engine::utils::Alignment::CENTER        : offset_ = glm::vec2{-  sprite_size_.x / 2.0f, -  sprite_size_.y / 2.0f} * scale; break;
    case engine::utils::Alignment::CENTER_RIGHT  : offset_ = glm::vec2{-  sprite_size_.x       , -  sprite_size_.y / 2.0f} * scale; break;
    case engine::utils::Alignment::BOTTOM_LEFT   : offset_ = glm::vec2{0.0f                    , -  sprite_size_.y       } * scale; break;
    case engine::utils::Alignment::BOTTOM_CENTER : offset_ = glm::vec2{-  sprite_size_.x / 2.0f, -  sprite_size_.y       } * scale; break;
    case engine::utils::Alignment::BOTTOM_RIGHT  : offset_ = glm::vec2{-  sprite_size_.x       , -  sprite_size_.y       } * scale; break;
    case engine::utils::Alignment::NONE:
    default:                                        break;
    }
}

void SpriteComponent::updateSpriteSize()
{
    if (!resource_manager_)
    {
        spdlog::error("SpriteComponent::updateSpriteSize: resource_manager_ is null");
        return;
    }

    if (sprite_.getSourceRect().has_value())
    {
        const auto& src_rect = sprite_.getSourceRect().value();
        sprite_size_ = {src_rect.w, src_rect.h};
    } else {
        sprite_size_ = resource_manager_->getTextureSize(sprite_.getTextureId());
    }
}

void SpriteComponent::init()
{
    if (!owner_)
    {
        spdlog::error("SpriteComponent::init: owner_ is null before init");
        return;
    }

    transform_ = owner_->getComponent<TransformComponent>();
    if (!transform_)
    {
        spdlog::warn("GmaeObject '{}' 上的 SpriteComponent 需要一个 TransformComponent, 但是未找到", owner_->getName());
        return;
    }

    updateSpriteSize();
    updateOffset();
}

void SpriteComponent::render(engine::core::Context &context)
{
    if (is_hidden_ || !transform_ || !resource_manager_)
    {
        return;
    }

    const glm::vec2& pos = transform_->getPosition() + offset_;
    const glm::vec2& scale = transform_->getScale();
    float rotation_degress = transform_->getRotation();

    context.getRenderer().drawSprite(context.getCamera(), sprite_, pos , scale, rotation_degress);
}

void SpriteComponent::setAlignment(engine::utils::Alignment alignment)
{
    alignment_ = alignment;
    updateOffset();
}

void SpriteComponent::setSpriteById(const std::string& texture_id, std::optional<SDL_FRect> source_rect_opt)
{
    sprite_.setTextureId(texture_id);
    sprite_.setSourceRect(source_rect_opt);

    updateSpriteSize();
    updateOffset();
}


void SpriteComponent::setSourceRect(const std::optional<SDL_FRect>& source_rect_opt)
{
    sprite_.setSourceRect(source_rect_opt);

    updateSpriteSize();
    updateOffset();
}

} // namspace engine::component