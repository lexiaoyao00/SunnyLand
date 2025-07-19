#include "physics_engine.h"
#include "../component/physics_component.h"
#include "../component/transform_component.h"
#include "../component/collider_component.h"
#include "../object/game_object.h"
#include "../physics/collision.h"
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

        collision_pairs_.clear();

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

            checkObjectCollision();

        }
    }

    void PhysicsEngine::checkObjectCollision()
    {
        for (size_t i = 0; i < components_.size(); ++i)
        {
            auto* pc_a = components_[i];
            if (!pc_a || !pc_a->isEnable()) continue; // Check if the component is valid and enabled
            auto* obj_a = pc_a->getOwner();
            if (!obj_a) continue;
            auto* cc_a = obj_a->getComponent<engine::component::ColliderComponent>();
            if (!cc_a || !cc_a->isActive()) continue;

            for (size_t j = i + 1; j < components_.size(); ++j){
                auto* pc_b = components_[j];
                if (!pc_b || !pc_b->isEnable()) continue; // Check if the component is valid and enabled
                auto* obj_b = pc_b->getOwner();
                if (!obj_b) continue;
                auto* cc_b = obj_b->getComponent<engine::component::ColliderComponent>();
                if (!cc_b || !cc_b->isActive()) continue;

                if (collision::checkCollision(*cc_a, *cc_b))
                {   // TODO:不是所有碰撞都需要插入 collision_pairs_ ，比如穿透，未来添加过滤条件
                    collision_pairs_.emplace_back(obj_a, obj_b);
                }
            }
        }
    }

}   // namespace engine::physics