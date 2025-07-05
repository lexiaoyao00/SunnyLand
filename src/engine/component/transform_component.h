#pragma once
#include "./component.h"
#include <glm/vec2.hpp>

namespace engine::component {

class TransformComponent final : public engine::component::Component {
    friend class engine::object::GameObject;

public:
    glm::vec2 position_ = {0.0f, 0.0f};
    glm::vec2 scale_ = {1.0f, 1.0f};
    float rotation_ = 0.0f;

    TransformComponent(glm::vec2 position = {0.0f, 0.0f}, glm::vec2 scale = {1.0f, 1.0f}, float rotation = 0.0f)
        : position_(position), scale_(scale), rotation_(rotation) {}

    TransformComponent(const TransformComponent&) = delete;
    TransformComponent& operator=(const TransformComponent&) = delete;
    TransformComponent(TransformComponent&&) = delete;
    TransformComponent& operator=(TransformComponent&&) = delete;

    void translate(const glm::vec2& offset) { position_ += offset;}         // 平移
    void setPosition(const glm::vec2& position) { position_ = position;}
    void setScale(const glm::vec2& scale);
    void setRotation(float rotation) { rotation_ = rotation;}
    const glm::vec2&  getPosition() const { return position_;}
    const glm::vec2&  getScale() const { return scale_;}
    float getRotation() const { return rotation_;}

private:
    void update(float, engine::core::Context&) override {}
};
}   // namespace engine::component