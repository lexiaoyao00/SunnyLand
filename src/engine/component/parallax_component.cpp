#include "parallax_component.h"
#include "transform_component.h"
#include "../render/renderer.h"
#include "../render/camera.h"
#include "../render/sprite.h"
#include "../object/game_object.h"
#include "../core/context.h"
#include <spdlog/spdlog.h>


namespace engine::component {

    ParallaxComponent::ParallaxComponent(const std::string &texture_id, const glm::vec2 &scroll_factor, const glm::bvec2 &repeat)
    : sprite_(engine::render::Sprite(texture_id)),
      scroll_factor_(scroll_factor),
      repeat_(repeat)
    {
        spdlog::trace("ParallaxComponent initialized, texture_id: {}", texture_id);
    }

    void ParallaxComponent::init()
    {
        if (!owner_)
        {
            spdlog::error("ParallaxComponent::init() called on uninitialized component");
            return;
        }

        transform_ = owner_->getComponent<TransformComponent>();
        if (!transform_)
        {
            spdlog::error("ParallaxComponent::init() called on uninitialized transform component");
            return;
        }
    }

    void ParallaxComponent::render(engine::core::Context& context)
    {
        if (is_hidden_ || !transform_) return;

        context.getRenderer().drawParallax(context.getCamera(), sprite_, transform_->getPosition(), scroll_factor_, repeat_);
    }

} // namespace engine::component