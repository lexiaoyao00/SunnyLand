#pragma once

#include "ui_state.h"

namespace engine::ui::state {
class UINormalState final : public UIState {
    friend class engine::ui::UIInteractive;

public:
    UINormalState(engine::ui::UIInteractive* owner) : UIState(owner) {}
    ~UINormalState() override = default;

    void enter() override;
    std::unique_ptr<UIState> handleInput(engine::core::Context& context) override;
};

}   // namespace engine::ui::state
