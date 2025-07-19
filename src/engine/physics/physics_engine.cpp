#include "physics_engine.h"
#include "../component/physics_component.h"
#include "../component/transform_component.h"
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>

namespace engine::physics {
    void PhysicsEngine::registerComponent(component::PhysicsComponent *component)
    {
        components_.push_back(component);
        spdlog::trace("PhysicsEngine::registerComponent() - Registered component");
    }

    void PhysicsEngine::unregisterComponent(component::PhysicsComponent *component)
    {
        auto it = std::remove(components_.begin(), components_.end(), component);
        components_.erase(it, components_.end());
        spdlog::trace("PhysicsEngine::unregisterComponent() - Unregistered component");
    }

    void PhysicsEngine::update(float delta_time)
    {
        for (auto* pc : components_)
        {
            if (!pc || !pc->isEnable()) // Check if the component is valid and enabled
            {
                continue;
            }

            if (pc->isUseGravity())
            {
                pc->addForce(gravity_ * pc->getMass());
            }
            /* 还可以添加其他力影响，如风力、摩擦力等，目前不考虑 */

            // 更新速度 v = v0 + a * t, a = F / m
            pc->velocity_ += (pc->getForce() / pc->getMass()) * delta_time;
            pc->clearForce();   //清除当前帧的力

            // 更新位置 s = s0 + v * t
            auto* tc = pc->getTransform();
            if (tc)
            {
                tc->translate(pc->velocity_ * delta_time);
            }

            // 最大速度限制
            pc->velocity_ = glm::clamp(pc->velocity_, -max_speed_, max_speed_);

        }
    }

}   // namespace engine::physics