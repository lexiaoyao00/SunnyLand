#include "ui_interactive.h"
#include <spdlog/spdlog.h>
#include "../core/context.h"
#include "../resource/resource_manager.h"
#include "../audio/audio_player.h"
#include "../render/renderer.h"

namespace engine::ui {
UIInteractive::UIInteractive(engine::core::Context &context, const glm::vec2 &position, const glm::vec2 &size)
    : UIElement(position, size),context_(context)
{
    spdlog::trace("UIInteractive created");
}

UIInteractive::~UIInteractive() = default;

void UIInteractive::addSprite(const std::string &name, std::unique_ptr<engine::render::Sprite> sprite)
{
    if (size_.x == 0.0f || size_.y == 0.0f) {
        size_ = context_.getResourceManager().getTextureSize(sprite->getTextureId());
    }

    sprite_[name] = std::move(sprite);
}

void UIInteractive::setSprite(const std::string &name)
{
    if (sprite_.find(name) != sprite_.end()) {
        current_sprite_ = sprite_[name].get();
    } else {
        spdlog::warn("尝试设置不存在的精灵: {}", name);
    }
}

void UIInteractive::addSound(const std::string &name, const std::string &path)
{
    sounds_[name] = path;
}

void UIInteractive::playSound(const std::string &name)
{
    if (sounds_.find(name) != sounds_.end()) {
        context_.getAudioPlayer().playSound(sounds_[name]);
    } else {
        spdlog::warn("尝试播放不存在的音效: {}", name);
    }
}

void UIInteractive::setState(std::unique_ptr<engine::ui::state::UIState> state)
{
    if (!state) {
        spdlog::warn("尝试设置空的状态");
        return;
    }

    state_ = std::move(state);
    state_->enter();
}

bool UIInteractive::handleInput(engine::core::Context &context)
{
    if (UIElement::handleInput(context)) {
        return true;
    }

    // 先更新子节点，再更新自己的状态
    if (state_ && interactive_) {
        if (auto next_state = state_->handleInput(context); next_state) {
            setState(std::move(next_state));
            return true;
        }
    }

    return false;
}

void UIInteractive::render(engine::core::Context &context)
{
    if (!visible_) return;

    // 先渲染自身再渲染子元素
    context.getRenderer().drawUISprite(*current_sprite_, getScreenPosition(), size_);

    UIElement::render(context);
}

} // namespace engine::ui
