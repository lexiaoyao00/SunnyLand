#include "collider_component.h"
#include "transform_component.h"
#include "../object/game_object.h"
#include "../physics/collider.h"
#include <spdlog/spdlog.h>

namespace engine::component {

ColliderComponent::ColliderComponent(std::unique_ptr<engine::physics::Collider> collider, engine::utils::Alignment alignment, bool is_trigger, bool is_active)
: collider_(std::move(collider)), alignment_(alignment), is_trigger_(is_trigger), is_active_(is_active)
{
    if (!collider_)
    {
        spdlog::error("ColliderComponent: collider is null");
    }
    spdlog::trace("Created ColliderComponent");
}

void ColliderComponent::updateOffset()
{
    if (!collider_) return;

    auto collider_size = collider_->getAABBSize();
    if (collider_size.x <= 0.0f || collider_size.y <= 0.0f)
    {
        offset_ = glm::vec2{0.0f, 0.0f};
        return;
    }

    auto scale = transform_->getScale();

    switch (alignment_)
    {
    case engine::utils::Alignment::TOP_LEFT      : offset_ = glm::vec2{0.0f                    , 0.0f                    } * scale; break;
    case engine::utils::Alignment::TOP_CENTER    : offset_ = glm::vec2{-  collider_size.x / 2.0f, 0.0f                    } * scale; break;
    case engine::utils::Alignment::TOP_RIGHT     : offset_ = glm::vec2{-  collider_size.x       , 0.0f                    } * scale; break;
    case engine::utils::Alignment::CENTER_LEFT   : offset_ = glm::vec2{0.0f                    , -  collider_size.y / 2.0f} * scale; break;
    case engine::utils::Alignment::CENTER        : offset_ = glm::vec2{-  collider_size.x / 2.0f, -  collider_size.y / 2.0f} * scale; break;
    case engine::utils::Alignment::CENTER_RIGHT  : offset_ = glm::vec2{-  collider_size.x       , -  collider_size.y / 2.0f} * scale; break;
    case engine::utils::Alignment::BOTTOM_LEFT   : offset_ = glm::vec2{0.0f                    , -  collider_size.y       } * scale; break;
    case engine::utils::Alignment::BOTTOM_CENTER : offset_ = glm::vec2{-  collider_size.x / 2.0f, -  collider_size.y       } * scale; break;
    case engine::utils::Alignment::BOTTOM_RIGHT  : offset_ = glm::vec2{-  collider_size.x       , -  collider_size.y       } * scale; break;
    case engine::utils::Alignment::NONE:
    default:                                        break;
    }
}

engine::utils::Rect ColliderComponent::getWorldAABB() const
{
    if (!transform_ || !collider_) return {glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f)};

    const glm::vec2 top_left_pos = transform_->getPosition() + offset_;
    const glm::vec2 base_size = collider_->getAABBSize();
    const glm::vec2 scale = transform_->getScale();
    glm::vec2 scaled_size = base_size * scale;

    return {top_left_pos, scaled_size};
}

void ColliderComponent::setAlignment(engine::utils::Alignment alignment)
{
    alignment_ = alignment;

    if (transform_ && collider_)
    {
        updateOffset();
    }
}

void ColliderComponent::init()
{
    if (!owner_)
    {
        spdlog::error("ColliderComponent: owner is null");
        return;
    }

    transform_ = owner_->getComponent<TransformComponent>();
    if (!transform_)
    {
        spdlog::error("ColliderComponent: transform is null");
        return;
    }

    updateOffset();
}

} // namespace engine::component