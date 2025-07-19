#include "physics_component.h"
#include "transform_component.h"
#include "../object/game_object.h"
#include "../physics/physics_engine.h"
#include <spdlog/spdlog.h>

namespace engine::component{
    PhysicsComponent::PhysicsComponent(engine::physics::PhysicsEngine *physics_engine, bool use_gravity, float mass)
    : physics_engine_(physics_engine), mass_(mass), use_gravity_(use_gravity)
    {
        if (!physics_engine_)
        {
            spdlog::error("PhysicsComponent: PhysicsEngine is null");
        }
        spdlog::trace("PhysicsComponent: created, use_gravity: {}, mass: {}", use_gravity_, mass_);
    }

    void PhysicsComponent::init()
    {
        if (!owner_)
        {
            spdlog::error("PhysicsComponent: owner_ is null");
            return;
        }
        if(!physics_engine_)
        {
            spdlog::error("PhysicsComponent: physics_engine_ is null");
            return;
        }

        transform_ = owner_->getComponent<TransformComponent>();
        if (!transform_)
        {
            spdlog::warn("PhysicsComponent: owner_ has no TransformComponent");
            return;
        }
        physics_engine_->registerComponent(this);
        spdlog::trace("PhysicsComponent: inited");
    }

    void PhysicsComponent::clean()
    {
        physics_engine_->unregisterComponent(this);
        spdlog::trace("PhysicsComponent: cleaned");
    }

} // namespace engine::component
