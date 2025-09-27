#pragma once
#include "player_state.h"


namespace game::component::state {

class HurtState : public PlayerState {
    friend class game::component::PlayerComponent;

private:
    float stunned_timer_ = 0.0f;

public:
    HurtState(PlayerComponent* player_component) : PlayerState(player_component) {}
    ~HurtState() override = default;

private:
    void enter() override;
    void exit() override;
    std::unique_ptr<PlayerState> handleInput(engine::core::Context&) override;
    std::unique_ptr<PlayerState> update(float delta_time, engine::core::Context&) override;
};

}   // namespace game::component::state