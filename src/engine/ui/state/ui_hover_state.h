#pragma once

#include "ui_state.h"

namespace engine::ui::state {
class UIHoverState final : public UIState {
    friend class engine::ui::UIInteractive;

public:
    UIHoverState(engine::ui::UIInteractive* owner) : UIState(owner) {}
    ~UIHoverState() override = default;

    void enter() override;
    std::unique_ptr<UIState> handleInput(engine::core::Context& context) override;
};

}   // namespace engine::ui::state
