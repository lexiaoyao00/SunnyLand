#include "ui_hover_state.h"
#include "ui_normal_state.h"
#include "ui_pressed_state.h"
#include "../ui_interactive.h"
#include "../../core/context.h"
#include "../../input/input_manager.h"
#include "../../audio/audio_player.h"
#include <spdlog/spdlog.h>


namespace engine::ui::state {

void UIHoverState::enter()
{
    owner_->setSprite("hover");
    spdlog::debug("切换到悬停状态");
}

std::unique_ptr<UIState> UIHoverState::handleInput(engine::core::Context& context)
{
    auto& input_manager = context.getInputManager();
    auto mouse_pos = input_manager.getLogicalMousePosition();
    if (!owner_->isPointInside(mouse_pos)) {
        return std::make_unique<engine::ui::state::UINormalState>(owner_);
    }
    if (input_manager.isActionPressed("MouseLeftClick")) {
        return std::make_unique<engine::ui::state::UIPressedState>(owner_);
    }

    return nullptr;
}
} // namespace engine::ui::state
