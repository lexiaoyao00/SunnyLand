#pragma once
#include "player_state.h"


namespace game::component::state {

class ClimbState : public PlayerState {
    friend class game::component::PlayerComponent;

public:
    ClimbState(PlayerComponent* player_component) : PlayerState(player_component) {}
    ~ClimbState() override = default;

private:
    void enter() override;
    void exit() override;
    std::unique_ptr<PlayerState> handleInput(engine::core::Context&) override;
    std::unique_ptr<PlayerState> update(float, engine::core::Context&) override;
};

}   // namespace game::component::state