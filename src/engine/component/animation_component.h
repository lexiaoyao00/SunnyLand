#pragma once
#include "component.h"
#include <unordered_map>
#include <string>
#include <memory>

namespace engine::render {
    class Animation;
}

namespace engine::component {
    class SpriteComponent;
}


namespace engine::component {

class AnimationComponent : public Component {
    friend class engine::object::GameObject;

private:
    std::unordered_map<std::string, std::unique_ptr<engine::render::Animation>> animations_;
    SpriteComponent* sprite_component_ = nullptr;   // 指向必须的 SpriteComponent 的指针
    engine::render::Animation* current_animation_ = nullptr;   // 当前正在播放的动画

    float animation_timer_ = 0.0f;   // 动画计时器
    bool is_playing_ = false;   // 是否正在播放动画
    bool is_one_shot_removeal_ = false;   // 是否在播放完一次动画后移除GameObject

public:
    AnimationComponent() = default;
    ~AnimationComponent() override;

    AnimationComponent(const AnimationComponent&) = delete;
    AnimationComponent& operator=(const AnimationComponent&) = delete;
    AnimationComponent(AnimationComponent&&) = delete;
    AnimationComponent& operator=(AnimationComponent&&) = delete;

    void addAnimation(std::unique_ptr<engine::render::Animation> animation);
    void playAnimation(const std::string& name);
    void stopAnimation() { is_playing_ = false; }

    // getters and setters
    std::string getCurrentAnimationName() const;
    bool isPlaying() const { return is_playing_; }
    bool isAnimationFinished() const;
    bool isOneShotRemoveal() const { return is_one_shot_removeal_; }
    void setOneShotRemoveal(bool value) { is_one_shot_removeal_ = value; }


protected:
    void init() override;
    void update(float, engine::core::Context&) override;
};

}   // namespace engine::component