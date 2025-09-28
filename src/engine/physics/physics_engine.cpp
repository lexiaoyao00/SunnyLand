#include "physics_engine.h"
#include "../component/physics_component.h"
#include "../component/transform_component.h"
#include "../component/collider_component.h"
#include "../component/tilelayer_component.h"
#include "../object/game_object.h"
#include "../physics/collision.h"
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <set>

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
        tile_trigger_events_.clear();

        for (auto* pc : components_)
        {
            if (!pc || !pc->isEnable()) // Check if the component is valid and enabled
            {
                continue;
            }

            pc->resetCollisionFlags(); // 重置碰撞标志位

            if (pc->isUseGravity())
            {
                pc->addForce(gravity_ * pc->getMass());
            }
            /* 还可以添加其他力影响，如风力、摩擦力等，目前不考虑 */

            // 更新速度 v = v0 + a * t, a = F / m
            pc->velocity_ += (pc->getForce() / pc->getMass()) * delta_time;
            pc->clearForce();   //清除当前帧的力

            // 处理瓦片层碰撞（速度和位置的更新也在此）
            resolveTileCollision(pc, delta_time);

            // 世界边缘处理
            applyWorldBounds(pc);

        }

        // 对象间的碰撞
        checkObjectCollision();

        // 检测瓦片触发事件（检测前已经处理完位移）
        checkTileTriggers();
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

                if (collision::checkCollision(*cc_a, *cc_b)) {
                    // 如果是可移动物体与SOLID物体碰撞，则直接处理位置变化，不用记录碰撞对
                    if (obj_a->getTag() != "solid" && obj_b->getTag() == "solid")
                    {
                        resolveSolidObjectCollision(obj_a, obj_b);
                    }
                    else if (obj_a->getTag() == "solid" && obj_b->getTag() != "solid")
                    {
                        resolveSolidObjectCollision(obj_b, obj_a);
                    }
                    else
                    {
                        collision_pairs_.emplace_back(obj_a, obj_b);
                    }
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
        if (!tc || !cc || cc->isTrigger()) return;
        auto world_aabb = cc->getWorldAABB();       // 使用最小包围盒进行碰撞检测（简化碰撞检测）
        auto obj_pos = world_aabb.position;
        auto obj_size = world_aabb.size;
        if (world_aabb.size.x <= 0.0f || world_aabb.size.y <= 0.0f) return;


        auto tolerance = 1.0f; // 检测右/下边缘时，需要减1像素，否则会检测到下一行/列的瓦片(地图瓦片位置序号从0开始，计算结果位置为2其实是1号瓦片)
        auto ds = pc->velocity_ * delta_time;   // 速度 * 时间 = 距离，计算移动距离
        auto new_obj_pos = obj_pos + ds;   // 新位置 = 旧位置 + 距离

        if (!cc->isActive()){   // 如果碰撞器未激活，则不进行碰撞检测，让物体正常移动然后返回
            tc->translate(ds);
            pc->velocity_ = glm::clamp(pc->velocity_, -max_speed_, max_speed_);
            return;
        }

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
                    pc->setCollidedRight(true);
                } else {
                    // 检测右下角斜坡瓦片
                    auto width_right = new_obj_pos.x + obj_size.x - tile_x * tile_size.x;
                    auto height_right = getTileHeightAtWidth(width_right, tile_type_bottom, tile_size);
                    if (height_right > 0.0f) {
                        // 如果有碰撞（角点的世界y坐标 > 斜坡地面的世界y坐标），就让物体贴着斜坡表面
                        if (new_obj_pos.y > (tile_y_bottom + 1) * layer->getTileSize().y - obj_size.y - height_right) {
                            new_obj_pos.y = (tile_y_bottom + 1) * layer->getTileSize().y - obj_size.y - height_right;
                            pc->setCollidedBelow(true);
                        }
                    }
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
                    pc->setCollidedLeft(true);
                } else {
                    // 检测左下角斜坡瓦片
                    auto width_left = new_obj_pos.x - tile_x * tile_size.x;
                    auto height_left = getTileHeightAtWidth(width_left, tile_type_bottom, tile_size);
                    if (height_left > 0.0f) {
                        if (new_obj_pos.y > (tile_y_bottom + 1) * layer->getTileSize().y - obj_size.y - height_left) {
                            new_obj_pos.y = (tile_y_bottom + 1) * layer->getTileSize().y - obj_size.y - height_left;
                            pc->setCollidedBelow(true);
                        }
                    }
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

                if (tile_type_left == engine::component::TileType::SOLID || tile_type_right == engine::component::TileType::SOLID ||
                    tile_type_left == engine::component::TileType::UNISOLID || tile_type_right == engine::component::TileType::UNISOLID) {
                    pc->velocity_.y = 0.0f;
                    new_obj_pos.y = tile_y * tile_size.y - obj_size.y;
                    pc->setCollidedBelow(true);
                } else {
                    // 检测下方斜坡瓦片
                    auto width_left = obj_pos.x - tile_x * tile_size.x;
                    auto width_right = obj_pos.x + obj_size.x - tile_x_right * tile_size.x;
                    auto height_left = getTileHeightAtWidth(width_left, tile_type_left, tile_size);
                    auto height_right = getTileHeightAtWidth(width_right, tile_type_right, tile_size);
                    auto height = std::max(height_left, height_right);  // 取左右两边的最高点进行检测
                    if (height > 0.0f) {
                        if (new_obj_pos.y > (tile_y + 1) * layer->getTileSize().y - obj_size.y - height) {
                            new_obj_pos.y = (tile_y + 1) * layer->getTileSize().y - obj_size.y - height;
                            pc->velocity_.y = 0.0f;     // 只有向下运动时才需要让 y 速度归零
                            pc->setCollidedBelow(true);
                        }
                    }
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
                    pc->setCollidedAbove(true);
                }
            }
        }
        // 更新对象位置，并限制最大速度

        tc->translate(new_obj_pos - obj_pos); // 使用 translate 方法，避免直接设置位置，因为碰撞盒可能有偏移量
        pc->velocity_ = glm::clamp(pc->velocity_, -max_speed_, max_speed_);
    }

    void PhysicsEngine::resolveSolidObjectCollision(engine::object::GameObject *move_obj, engine::object::GameObject *solid_obj)
    {
        auto* move_tc = move_obj->getComponent<engine::component::TransformComponent>();
        auto* move_pc = move_obj->getComponent<engine::component::PhysicsComponent>();
        auto* move_cc = move_obj->getComponent<engine::component::ColliderComponent>();
        auto* solid_cc = solid_obj->getComponent<engine::component::ColliderComponent>();

        // 这里只获取期望位置，无法获取当前帧初始位置，无法进行轴分离碰撞检测
        auto move_aabb = move_cc->getWorldAABB();
        auto solid_aabb = solid_cc->getWorldAABB();

        // 使用最小平移向量解决碰撞问题
        auto move_center = move_aabb.position + move_aabb.size / 2.0f;
        auto solid_center = solid_aabb.position + solid_aabb.size / 2.0f;
        auto overlap = glm::vec2(move_aabb.size/2.0f + solid_aabb.size/2.0f) - glm::abs(move_center - solid_center);
        if (overlap.x < 0.1f && overlap.y < 0.1f) return; // 重叠部分太小，则认为没有碰撞

        if (overlap.x < overlap.y) { // X轴重叠更多，优先解决X轴碰撞
            if (move_center.x < solid_center.x) {
                move_tc->translate(glm::vec2(-overlap.x, 0.0f));
                if (move_pc->velocity_.x > 0.0f)
                {
                    move_pc->velocity_.x = 0.0f;
                    move_pc->setCollidedRight(true);
                }
            } else { //
                move_tc->translate(glm::vec2(overlap.x, 0.0f));
                if (move_pc->velocity_.x < 0.0f)
                {
                    move_pc->velocity_.x = 0.0f;
                    move_pc->setCollidedLeft(true);
                }
            }
        } else { // Y轴重叠更多，优先解决Y轴碰撞
            if (move_center.y < solid_center.y) {
                move_tc->translate(glm::vec2(0.0f, -overlap.y));
                if (move_pc->velocity_.y > 0.0f)
                {
                    move_pc->velocity_.y = 0.0f;
                    move_pc->setCollidedBelow(true);
                }
            }
            else {
                move_tc->translate(glm::vec2(0.0f, overlap.y));
                if (move_pc->velocity_.y < 0.0f)
                {
                    move_pc->velocity_.y = 0.0f;
                    move_pc->setCollidedAbove(true);
                }
            }
        }
    }

    void PhysicsEngine::applyWorldBounds(engine::component::PhysicsComponent *pc)
    {
        if (!pc || !world_bounds_) return;

        // 只限定左、上、右边界，不限定下边界，以碰撞盒作为判断依据
        auto* obj = pc->getOwner();
        auto* cc = obj->getComponent<engine::component::ColliderComponent>();
        auto* tc = obj->getComponent<engine::component::TransformComponent>();
        auto world_aabb = cc->getWorldAABB();
        auto obj_pos = world_aabb.position;
        auto obj_size = world_aabb.size;

        if (obj_pos.x < world_bounds_->position.x){
            pc->velocity_.x = 0.0f;
            obj_pos.x = world_bounds_->position.x;
        }
        if (obj_pos.y < world_bounds_->position.y){
            pc->velocity_.y = 0.0f;
            obj_pos.y = world_bounds_->position.y;
        }
        if (obj_pos.x + obj_size.x > world_bounds_->position.x + world_bounds_->size.x){
            pc->velocity_.x = 0.0f;
            obj_pos.x = world_bounds_->position.x + world_bounds_->size.x - obj_size.x;
        }

        tc->translate(obj_pos - world_aabb.position);
    }

    float PhysicsEngine::getTileHeightAtWidth(float width, engine::component::TileType type, glm::vec2 tile_size)
    {
        auto rel_x = glm::clamp(width / tile_size.x, 0.0f, 1.0f);
        switch (type)
        {
            case engine::component::TileType::SLOPE_0_1:    // 斜坡 0-1
                return rel_x * tile_size.y;
            case engine::component::TileType::SLOPE_0_2:    // 斜坡 0-2
                return rel_x * tile_size.y * 0.5f;
            case engine::component::TileType::SLOPE_2_1:    // 斜坡 2-1
                return rel_x * tile_size.y * 0.5f + tile_size.y * 0.5f;
            case engine::component::TileType::SLOPE_1_0:    // 斜坡 1-0
                return (1.0f - rel_x) * tile_size.y;
            case engine::component::TileType::SLOPE_2_0:    // 斜坡 2-0
                return (1.0f - rel_x) * tile_size.y * 0.5f;
            case engine::component::TileType::SLOPE_1_2:    // 斜坡 1-2
                return (1.0f - rel_x) * tile_size.y * 0.5f + tile_size.y * 0.5f;
            default:
                return 0.0f;        // 默认返回0.表示没有斜坡
        }
    }

    void PhysicsEngine::checkTileTriggers()
    {
        for (auto* pc : components_){
            if (!pc || !pc->isEnable()) continue;
            auto* obj = pc->getOwner();
            if (!obj) continue;
            auto* cc = obj->getComponent<engine::component::ColliderComponent>();
            if (!cc || !cc->isActive() || cc->isTrigger()) continue;

            auto world_aabb = cc->getWorldAABB();

            std::set<engine::component::TileType> triggers_set;

            for (auto* layer:collision_tile_layers_){
                if (!layer) continue;
                auto tile_size = layer->getTileSize();
                constexpr float tolerance = 1.0f;   // 检测右边缘和下边缘时，需要减1像素，否则会检测不到

                // 获取瓦片坐标范围
                auto start_x = static_cast<int>(floor(world_aabb.position.x / tile_size.x));
                auto start_y = static_cast<int>(floor(world_aabb.position.y / tile_size.y));
                auto end_x = static_cast<int>(ceil((world_aabb.position.x + world_aabb.size.x - tolerance) / tile_size.x));
                auto end_y = static_cast<int>(ceil((world_aabb.position.y + world_aabb.size.y - tolerance) / tile_size.y));

                for (int x = start_x; x <= end_x; ++x){
                    for (int y = start_y; y <= end_y; ++y){
                        auto tile_type = layer->getTileTypeAt({x, y});
                        // TODO: 添加更多触发器类型
                        if (tile_type  == engine::component::TileType::HAZARD){
                            triggers_set.insert(tile_type);
                        }
                    }
                }

                for (const auto& type : triggers_set){
                    tile_trigger_events_.emplace_back(obj, type);
                    spdlog::trace("Tile trigger event: obj={}, type={}", obj->getName(), static_cast<int>(type));
                }
            }
        }
    }

}   // namespace engine::physics