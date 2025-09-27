#include "dead_state.h"
#include "idle_state.h"
#include "walk_state.h"
#include "fall_state.h"
#include "../player_component.h"
#include "../../../engine/component/physics_component.h"
#include "../../../engine/component/collider_component.h"
#include "../../../engine/object/game_object.h"


#include <spdlog/spdlog.h>

namespace game::component::state {

    void DeadState::enter()
    {
        spdlog::debug("玩家进入死亡状态");
        playAnimation("hurt");
        auto physics_component = player_component_->getPhysicsComponent();
        physics_component->velocity_ = glm::vec2(0.0f, -200.0f);    // 向上击退

        auto collider_component = player_component_->getOwner()->getComponent<engine::component::ColliderComponent>();
        if (collider_component)
        {
            collider_component->setActive(false);
        }
    }

    void DeadState::exit()
    {
    }

    std::unique_ptr<PlayerState> DeadState::handleInput(engine::core::Context &)
    {
        // 死亡状态不处理输入
        return nullptr;
    }

    std::unique_ptr<PlayerState> DeadState::update(float, engine::core::Context &)
    {
        // 死亡状态不更新状态
        return nullptr;
    }

}   // namespace game::component::state
