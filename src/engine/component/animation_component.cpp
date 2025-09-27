#include "animation_component.h"
#include "../object/game_object.h"
#include "../render/animation.h"
#include "sprite_component.h"
#include <spdlog/spdlog.h>


namespace engine::component {
    AnimationComponent::~AnimationComponent() = default;

    void AnimationComponent::init()
    {
        if (!owner_){
            spdlog::error("AnimationComponent::init() - owner_ is nullptr");
            return;
        }

        sprite_component_ = owner_->getComponent<SpriteComponent>();
        if (!sprite_component_){
            spdlog::error("AnimationComponent::init() - sprite_component_ is nullptr on GameObject: {}",owner_->getName());
            return;
        }
    }

    void AnimationComponent::update(float delta_time, engine::core::Context &)
    {
        if (!is_playing_ || !current_animation_ || !sprite_component_ || current_animation_->isEmpty()){
            spdlog::trace("AnimationComponent::update() - no playing animation or current_animation_ is nullptr or empty");
            return;
        }

        animation_timer_ += delta_time;

        const auto& current_frame = current_animation_->getFrame(animation_timer_);
        sprite_component_->setSourceRect(current_frame.source_rect);

        if (!current_animation_->isLooping() && animation_timer_ >= current_animation_->getTotalDuration()){
            is_playing_ = false;
            animation_timer_ = current_animation_->getTotalDuration();
            if (is_one_shot_removeal_){
                owner_->setNeedRemove(true);
            }
        }
    }

    void AnimationComponent::addAnimation(std::unique_ptr<engine::render::Animation> animation)
    {
        if (!animation) return;
        std::string name = animation->getName();
        animations_[name] = std::move(animation);
        spdlog::debug("AnimationComponent::addAnimation() - added animation: {} to GameObject: {}", name, owner_ ? owner_->getName() : "Unknown GameObject");
    }

    void AnimationComponent::playAnimation(const std::string &name)
    {
        auto it = animations_.find(name);
        if (it == animations_.end() || !it->second){
            spdlog::error("AnimationComponent::playAnimation() - animation: {} of GameObject {} not found", name, owner_ ? owner_->getName() : "Unknown GameObject");
            return;
        }

        // 已经在播放相同的动画，不重新开始播放
        if (current_animation_ == it->second.get() && is_playing_){
            return;
        }

        current_animation_ = it->second.get();
        animation_timer_ = 0.0f;
        is_playing_ = true;

        if (sprite_component_ && !current_animation_->isEmpty()){
            const auto& first_frame = current_animation_->getFrame(0.0f);
            sprite_component_->setSourceRect(first_frame.source_rect);
            spdlog::debug("AnimationComponent::playAnimation() - playing animation: {} of GameObject: {}", name, owner_ ? owner_->getName() : "Unknown GameObject");
        }
    }

    std::string AnimationComponent::getCurrentAnimationName() const
    {
        if(current_animation_){
            return current_animation_->getName();
        }
        return "";
    }

    bool AnimationComponent::isAnimationFinished() const
    {
        if (!current_animation_ || current_animation_->isEmpty()){
            return false;
        }
        return animation_timer_ >= current_animation_->getTotalDuration();
    }

} // namespace engine::component