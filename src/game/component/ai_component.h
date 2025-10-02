#pragma once

#include "../../engine/component/component.h"
#include "ai/ai_behavior.h"
#include <memory>


namespace game::component::ai {class AIBehavior;}
namespace engine::component {
    class TransformComponent;
    class PhysicsComponent;
    class SpriteComponent;
    class AnimationComponent;
    class AudioComponent;
}

namespace game::component {

class AIComponent : public engine::component::Component {
    friend class engine::object::GameObject;

private:
    std::unique_ptr<ai::AIBehavior> current_behavior_ = nullptr;
    // TODO:可以添加一些敌人属性

    // 缓存组件指针
    engine::component::TransformComponent* transform_component_ = nullptr;
    engine::component::PhysicsComponent* physics_component_ = nullptr;
    engine::component::SpriteComponent* sprite_component_ = nullptr;
    engine::component::AnimationComponent* animation_component_ = nullptr;
    engine::component::AudioComponent* audio_component_ = nullptr;


public:
    AIComponent() = default;
    ~AIComponent() override = default;

    AIComponent(const AIComponent&) = delete;
    AIComponent& operator=(const AIComponent&) = delete;
    AIComponent(AIComponent&&) = delete;
    AIComponent& operator=(AIComponent&&) = delete;

    void setBehavior(std::unique_ptr<ai::AIBehavior> behavior); // 设置当前 AI 行为策略
    bool takeDamage(int damage); // 处理伤害逻辑，返回是否造成伤害
    bool isAlive() const; // 判断 AI 是否存活

    // getters and setters
    engine::component::TransformComponent* getTransformComponent() const { return transform_component_;}
    engine::component::PhysicsComponent* getPhysicsComponent() const { return physics_component_;}
    engine::component::SpriteComponent* getSpriteComponent() const { return sprite_component_;}
    engine::component::AnimationComponent* getAnimationComponent() const { return animation_component_;}
    engine::component::AudioComponent* getAudioComponent() const { return audio_component_;}

private:
    void init() override;
    void update(float delta_time, engine::core::Context&) override;

};

} // game::component