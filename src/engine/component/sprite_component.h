#pragma once
#include "./component.h"
#include "../render/sprite.h"
#include "../utils/alignment.h"
#include <string>
#include <optional>
#include <SDL3/SDL_Rect.h>
#include <glm/vec2.hpp>


namespace engine::core {
    class Context;
}

namespace engine::resource {
    class ResourceManager;
}

namespace engine::component {
    class TransformComponent;

    class SpriteComponent final : public engine::component::Component{
        friend class engine::object::GameObject;        // 友元不能继承
    private:
        engine::resource::ResourceManager* resource_manager_ = nullptr;
        TransformComponent* transform_ = nullptr;

        engine::render::Sprite sprite_;
        engine::utils::Alignment alignment_  = engine::utils::Alignment::NONE;
        glm::vec2 sprite_size_ = {0.0f,0.0f};
        glm::vec2 offset_ = {0.0f,0.0f};
        bool is_hidden_ = false;

    public:
        SpriteComponent(
            const std::string& texture_id,
            engine::resource::ResourceManager& resource_manager,
            engine::utils::Alignment alignment = engine::utils::Alignment::NONE,
            std::optional<SDL_FRect> source_rect_opt = std::nullopt,
            bool is_flipped = false
        );

        SpriteComponent(
            engine::render::Sprite&& sprite,
            engine::resource::ResourceManager& resource_manager,
            engine::utils::Alignment alignment = engine::utils::Alignment::NONE
        );
        ~SpriteComponent() override = default;

        SpriteComponent(const SpriteComponent&) = delete;
        SpriteComponent& operator=(const SpriteComponent&) = delete;
        SpriteComponent(SpriteComponent&&) = delete;
        SpriteComponent& operator=(SpriteComponent&&) = delete;

        void updateOffset();        // 更新偏移量（根据当前的 alignment_, sprite_size_ 计算 offset_）

        // getters
        engine::render::Sprite* getSprite() { return &sprite_; }
        const std::string& getTextureId() { return sprite_.getTextureId(); }
        engine::utils::Alignment getAlignment() { return alignment_; }
        const glm::vec2& getSpriteSize() { return sprite_size_; }
        const glm::vec2& getOffset() { return offset_; }
        bool isHidden() { return is_hidden_; }
        bool isFlipped() { return sprite_.isFlipped(); }

        // setters
        void setSpriteById(const std::string& texture_id, std::optional<SDL_FRect> source_rect_opt = std::nullopt);
        void setAlignment(engine::utils::Alignment alignment);
        void setSourceRect(const std::optional<SDL_FRect>& source_rect_opt);
        void setFlipped(bool is_flipped) { sprite_.setFlipped(is_flipped); }
        void setHidden(bool is_hidden) { is_hidden_ = is_hidden; }

    private:
        void updateSpriteSize();

        // Component 接口
        void init() override;
        void update(float, engine::core::Context&) override {}
        void render(engine::core::Context&) override;


};
}   // namespace engine::component