#include "ui_hover_state.h"
#include "ui_normal_state.h"
#include "ui_pressed_state.h"
#include "../ui_interactive.h"
#include "../../core/context.h"
#include "../../input/input_manager.h"
#include "../../audio/audio_player.h"
#include <spdlog/spdlog.h>


namespace engine::ui::state {

void UIPressedState::enter()
{
    owner_->setSprite("pressed");
    owner_->playSound("pressed");
    spdlog::debug("切换到按下状态");
}

std::unique_ptr<UIState> UIPressedState::handleInput(engine::core::Context& context)
{
    auto& input_manager = context.getInputManager();
    auto mouse_pos = input_manager.getLogicalMousePosition();
    if(input_manager.isActionReleased("MouseLeftClick")) {
        if (!owner_->isPointInside(mouse_pos)) {
            return std::make_unique<engine::ui::state::UINormalState>(owner_);
        } else {
            owner_->clicked();
            return std::make_unique<engine::ui::state::UIHoverState>(owner_);
        }
    }

    return nullptr;
}
} // namespace engine::ui::state
