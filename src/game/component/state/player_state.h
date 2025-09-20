#pragma once
#include <memory>

namespace engine::core {
    class Context;
}

namespace game::component {
    class PlayerComponent;
}


namespace game::component::state {

/**
 * @brief 玩家状态机的抽象基类
 *
 */
class PlayerState {
    friend class game::component::PlayerComponent;

protected:
    PlayerComponent* player_component_ = nullptr;       // 指向拥有此状态的玩家组件

public:
    PlayerState(PlayerComponent* player_component) : player_component_(player_component) {}
    virtual ~PlayerState() = default;

    PlayerState(const PlayerState&) = delete;
    PlayerState& operator=(const PlayerState&) = delete;
    PlayerState(PlayerState&&) = delete;
    PlayerState& operator=(PlayerState&&) = delete;


protected:
    // 核心状态方法
    virtual void enter() = 0;
    virtual void exit() = 0;
    virtual std::unique_ptr<PlayerState> handleInput(engine::core::Context&) = 0;
    virtual std::unique_ptr<PlayerState> update(float, engine::core::Context&) = 0;
    /* handleInput 和 update 方法返回下一个状态，如果不需要切换状态则返回 nullptr  */
};


}   // namespace game::component::state