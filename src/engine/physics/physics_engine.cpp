#include "physics_engine.h"
#include "../component/physics_component.h"
#include "../component/transform_component.h"
#include "../component/collider_component.h"
#include "../component/tilelayer_component.h"
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

    void PhysicsEngine::registerCollisionTileLayer(component::TileLayerComponent *tile_layer)
    {
        tile_layer->setPhysicsEngine(this); // 设置物理引擎指针
        collision_tile_layers_.push_back(tile_layer);
        spdlog::trace("PhysicsEngine::registerCollisionTileLayer() - Registered collision tile layer");
    }

    void PhysicsEngine::unregisterCollisionTileLayer(component::TileLayerComponent *tile_layer)
    {
        auto it = std::remove(collision_tile_layers_.begin(), collision_tile_layers_.end(), tile_layer);
        collision_tile_layers_.erase(it, collision_tile_layers_.end());
        spdlog::trace("PhysicsEngine::unregisterCollisionTileLayer() - Unregistered collision tile layer");
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

            resolveTileCollision(pc, delta_time);

        }

        checkObjectCollision();
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

    void PhysicsEngine::resolveTileCollision(engine::component::PhysicsComponent *pc, float delta_time)
    {
        // 检查组件是否有效
        auto* obj = pc->getOwner();
        if (!obj) return;
        auto* tc = obj->getComponent<engine::component::TransformComponent>();
        auto* cc = obj->getComponent<engine::component::ColliderComponent>();
        if (!tc || !cc || !cc->isActive() || cc->isTrigger()) return;
        auto world_aabb = cc->getWorldAABB();       // 使用最小包围盒进行碰撞检测（简化碰撞检测）
        auto obj_pos = world_aabb.position;
        auto obj_size = world_aabb.size;
        if (world_aabb.size.x <= 0.0f || world_aabb.size.y <= 0.0f) return;


        auto tolerance = 1.0f; // 检测右/下边缘时，需要减1像素，否则会检测到下一行/列的瓦片(地图瓦片位置序号从0开始，计算结果位置为2其实是1号瓦片)
        auto ds = pc->velocity_ * delta_time;   // 速度 * 时间 = 距离，计算移动距离
        auto new_obj_pos = obj_pos + ds;   // 新位置 = 旧位置 + 距离

        for (auto* layer : collision_tile_layers_){
            if (!layer) continue;

            auto tile_size = layer->getTileSize();
            // 期望位置计算，用于检测碰撞
            if (ds.x > 0.0f) {  // 向右移动
                // 计算新位置的右边缘
                auto right_top_x = new_obj_pos.x + obj_size.x;
                auto tile_x = static_cast<int>(floor(right_top_x / tile_size.x));   // 获取x方向瓦片坐标
                auto tile_y = static_cast<int>(floor(obj_pos.y / tile_size.y));       // 获取y方向瓦片右上坐标
                auto tile_type_top = layer->getTileTypeAt({tile_x, tile_y});
                auto tile_y_bottom = static_cast<int>(floor((obj_pos.y + obj_size.y - tolerance) / tile_size.y));   // 获取y方向瓦片右下坐标
                auto tile_type_bottom = layer->getTileTypeAt({tile_x, tile_y_bottom});

                if (tile_type_top == engine::component::TileType::SOLID || tile_type_bottom == engine::component::TileType::SOLID) {
                    // 碰撞了，停止移动
                    pc->velocity_.x = 0.0f;
                    new_obj_pos.x = tile_x * tile_size.x - obj_size.x;
                }
            }
            else if (ds.x < 0.0f) {  // 向左移动
                // 计算新位置的左边缘
                auto left_top_x = new_obj_pos.x;
                auto tile_x = static_cast<int>(floor(left_top_x / tile_size.x));   // 获取x方向瓦片坐标
                auto tile_y = static_cast<int>(floor(obj_pos.y / tile_size.y));       // 获取y方向瓦片右上坐标
                auto tile_type_top = layer->getTileTypeAt({tile_x, tile_y});    // 左上角瓦片类型
                auto tile_y_bottom = static_cast<int>(floor((obj_pos.y + obj_size.y - tolerance) / tile_size.y));   // 获取y方向瓦片右下坐标
                auto tile_type_bottom = layer->getTileTypeAt({tile_x, tile_y_bottom});  //左下角瓦片类型

                if (tile_type_top == engine::component::TileType::SOLID || tile_type_bottom == engine::component::TileType::SOLID) {
                    pc->velocity_.x = 0.0f;
                    new_obj_pos.x = (tile_x + 1) * tile_size.x;
                }
            }
            // 轴分离碰撞检测：再检测Y方向是否碰撞（X方向使用初始值 obj_pos.x）
            if (ds.y > 0.0f) {  // 向下移动
                // 检测底部碰撞，测试左下角和右下角
                auto botton_left_y = new_obj_pos.y + obj_size.y;
                auto tile_y = static_cast<int>(floor(botton_left_y / tile_size.y));   // 获取y方向瓦片坐标
                auto tile_x = static_cast<int>(floor(obj_pos.x / tile_size.x));       // 获取x方向瓦片左下坐标
                auto tile_type_left = layer->getTileTypeAt({tile_x, tile_y});
                auto tile_x_right = static_cast<int>(floor((obj_pos.x + obj_size.x - tolerance) / tile_size.x));   // 获取x方向瓦片右下坐标
                auto tile_type_right = layer->getTileTypeAt({tile_x_right, tile_y});

                if (tile_type_left == engine::component::TileType::SOLID || tile_type_right == engine::component::TileType::SOLID) {
                    pc->velocity_.y = 0.0f;
                    new_obj_pos.y = tile_y * tile_size.y - obj_size.y;
                }
            }
            else if (ds.y < 0.0f) {  // 向上移动
                // 检测顶部碰撞，测试左上角和右上角
                auto top_left_y = new_obj_pos.y;
                auto tile_y = static_cast<int>(floor(top_left_y / tile_size.y));   // 获取y方向瓦片坐标
                auto tile_x = static_cast<int>(floor(obj_pos.x / tile_size.x));       // 获取x方向瓦片左上坐标
                auto tile_type_left = layer->getTileTypeAt({tile_x, tile_y});
                auto tile_x_right = static_cast<int>(floor((obj_pos.x + obj_size.x - tolerance) / tile_size.x));   // 获取x方向瓦片右上坐标
                auto tile_type_right = layer->getTileTypeAt({tile_x_right, tile_y});

                if (tile_type_left == engine::component::TileType::SOLID || tile_type_right == engine::component::TileType::SOLID) {
                    pc->velocity_.y = 0.0f;
                    new_obj_pos.y = (tile_y + 1) * tile_size.y;
                }
            }
        }
        // 更新对象位置，并限制最大速度

        tc->setPosition(new_obj_pos);
        pc->velocity_ = glm::clamp(pc->velocity_, -max_speed_, max_speed_);
    }

}   // namespace engine::physics