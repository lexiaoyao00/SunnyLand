#include "game_scene.h"
#include "../../engine/object/game_object.h"
#include "../../engine/component/transform_component.h"
#include "../../engine/component/sprite_component.h"
#include "../../engine/component/physics_component.h"
#include "../../engine/component/collider_component.h"
#include "../../engine/component/tilelayer_component.h"
#include "../../engine/component/animation_component.h"
#include "../../engine/component/health_component.h"
#include "../../engine/core/context.h"
#include "../../engine/scene/level_loader.h"
#include "../../engine/input/input_manager.h"
#include "../../engine/render/camera.h"
#include "../../engine/render/animation.h"
#include "../../engine/physics/physics_engine.h"
#include "../../engine/utils/math.h"

#include "../component/player_component.h"

#include <spdlog/spdlog.h>

namespace game::scene {

GameScene::GameScene(std::string name, engine::core::Context &context, engine::scene::SceneManager &scene_manager)
: Scene(name, context, scene_manager)
{
    spdlog::trace("GameScene constructor");
}

void GameScene::init()
{
    if (is_initialized_) {
        spdlog::warn("GameScene has been initialized, do not initialize again");
        return;
    }
    spdlog::trace("GameScene init start");

    if (!initLevel()){
        spdlog::error("Failed to initialize level,cannot countinue...");
        context_.getInputManager().setShouldQuit(true);
        return;
    }
    if (!initPlayer()) {
        spdlog::error("Failed to initialize player,cannot countinue...");
        context_.getInputManager().setShouldQuit(true);
        return;
    }
    if (!initEnemyAndItem()) {
        spdlog::error("Failed to initialize enemy and item,cannot countinue...");
        context_.getInputManager().setShouldQuit(true);
        return;
    }


    Scene::init();
    spdlog::trace("GameScene has been initialized");
}

void GameScene::update(float delta_time)
{
    // TODO:
    Scene::update(delta_time);
    handleObjectCollisions();
    handleTileTriggers();
}

void GameScene::render()
{
    // TODO:
    Scene::render();
}

void GameScene::handleInput()
{
    // TODO:
    Scene::handleInput();
}

void GameScene::clean()
{
    // TODO:
    Scene::clean();
    spdlog::trace("GameScene has been cleaned");
}

bool GameScene::initLevel()
{
    engine::scene::LevelLoader level_loader;
    if (!level_loader.loadLevel("assets/maps/level1.tmj", this)) {
        spdlog::error("Failed to load level");
        return false;
    }

    // 注册 main 注册到物理引擎
    auto* main_layer = findGameObjectByName("main");
    if (!main_layer) {
        spdlog::error("Main layer not found");
        return false;
    }

    auto* tile_layer = main_layer->getComponent<engine::component::TileLayerComponent>();
    if (!tile_layer) {
        spdlog::error("Tile layer not found");
        return false;
    }

    context_.getPhysicsEngine().registerCollisionTileLayer(tile_layer);
    spdlog::info("main layer has been registered to physics engine");

    // 设置相机边界
    auto world_size = main_layer->getComponent<engine::component::TileLayerComponent>()->getWorldSize();
    context_.getCamera().setLimitBounds(engine::utils::Rect(glm::vec2(0.0f), world_size));

    // 设置世界边界
    context_.getPhysicsEngine().setWorldBounds(engine::utils::Rect(glm::vec2(0.0f), world_size));

    spdlog::trace("GameScene has been initialized");
    return true;
}

bool GameScene::initPlayer()
{
    // 创建测试对象
    player_ = findGameObjectByName("player");
    if (!player_) {
        spdlog::error("Player not found");
        return false;
    }

    // 添加 PlayerComponent 到玩家
    auto* player_component = player_->addComponent<game::component::PlayerComponent>();
    if (!player_component) {
        spdlog::error("Failed to add PlayerComponent to player");
        return false;
    }

    // 相机跟随玩家
    auto* player_transform = player_->getComponent<engine::component::TransformComponent>();
    if (!player_transform) {
        spdlog::error("Player transform not found");
        return false;
    }
    context_.getCamera().setTarget(player_transform);

    spdlog::trace("Player has been initialized");
    return true;
}

bool GameScene::initEnemyAndItem()
{
    bool success = true;
    for (auto& game_object : game_objects_){
        if (game_object->getName() == "eagle")
        {
            if (auto* ac = game_object->getComponent<engine::component::AnimationComponent>(); ac){
                ac->playAnimation("fly");
            } else {
                spdlog::error(" Eagle 对象缺少 AnimationComponent，无法播放动画。");
                success = false;
            }
        }
        if (game_object->getName() == "frog"){
            if (auto* ac = game_object->getComponent<engine::component::AnimationComponent>(); ac){
                ac->playAnimation("idle");
            } else {
                spdlog::error(" Frog 对象缺少 AnimationComponent，无法播放动画。");
                success = false;
            }
        }
        if (game_object->getName() == "opossum"){
            if (auto* ac = game_object->getComponent<engine::component::AnimationComponent>(); ac){
                ac->playAnimation("walk");
            } else {
                spdlog::error(" Opossum 对象缺少 AnimationComponent，无法播放动画。");
                success = false;
            }
        }
        if (game_object->getTag() == "item"){
            if (auto* ac = game_object->getComponent<engine::component::AnimationComponent>(); ac){
                ac->playAnimation("idle");
            } else {
                spdlog::error(" Item 对象缺少 AnimationComponent，无法播放动画。");
                success = false;
            }
        }
    }

    return success;
}

void GameScene::handleObjectCollisions()
{
    auto collision_pairs = context_.getPhysicsEngine().getCollisionPairs();
    for (const auto& pair:collision_pairs){
        auto* obj1 = pair.first;
        auto* obj2 = pair.second;

        // 玩家与敌人的碰撞
        if (obj1->getName() == "player" && obj2->getTag() == "enemy"){
            PlayerVSEnemyCollision(obj1,obj2);
        } else if (obj1->getTag() == "enemy" && obj2->getName() == "player"){
            PlayerVSEnemyCollision(obj2,obj1);
        }
        // 玩家与道具的碰撞
        else if (obj1->getName() == "player" && obj2->getTag() == "item"){
            PlayerVSItemCollision(obj1,obj2);
        } else if (obj1->getTag() == "item" && obj2->getName() == "player"){
            PlayerVSItemCollision(obj2,obj1);
        }
        // 玩家与"hazard"瓦片的碰撞
        else if (obj1->getName() == "player" && obj2->getTag() == "hazard"){
            obj1->getComponent<game::component::PlayerComponent>()->takeDamage(1);
        } else if (obj1->getTag() == "hazard" && obj2->getName() == "player"){
            obj1->getComponent<game::component::PlayerComponent>()->takeDamage(1);
        }
    }
}

void GameScene::handleTileTriggers()
{
    const auto& tile_trigger_events = context_.getPhysicsEngine().getTileTriggerEvents();
    for (const auto& event : tile_trigger_events) {
        auto* obj = event.first;
        auto tile_type = event.second;
        if (tile_type == engine::component::TileType::HAZARD){
            if (obj->getName() == "player"){
                obj->getComponent<game::component::PlayerComponent>()->takeDamage(1);
                spdlog::debug("玩家 {} 受到 HAZARD 瓦片伤害", obj->getName());
            }
            // TODO: 其他对象类型的处理，目前让敌人无视瓦片伤害
        }
    }
}

void GameScene::PlayerVSEnemyCollision(engine::object::GameObject *player, engine::object::GameObject *enemy)
{
    auto player_aabb = player->getComponent<engine::component::ColliderComponent>()->getWorldAABB();
    auto enemy_aabb = enemy->getComponent<engine::component::ColliderComponent>()->getWorldAABB();
    auto player_center = player_aabb.position + player_aabb.size / 2.0f;
    auto enemy_center = enemy_aabb.position + enemy_aabb.size / 2.0f;
    auto overlap = glm::vec2(player_aabb.size / 2.0f + enemy_aabb.size / 2.0f) - glm::abs(player_center - enemy_center);


    // 踩踏敌人
    if (overlap.x > overlap.y && player_center.y < enemy_center.y){
        spdlog::info("玩家 {} 踩踏了敌人 {}",player->getName(),enemy->getName());
        auto enemy_health = enemy->getComponent<engine::component::HealthComponent>();
        if (!enemy_health){
            spdlog::error("敌人 {} 没有 HealthComponent 组件， 无法处理踩踏伤害",enemy->getName());
            return;
        }
        enemy_health->takeDamage(1);    // 造成 1 点伤害
        if (!enemy_health->isAlive()){
            spdlog::info("敌人 {} 被踩踏后死亡",enemy->getName());
            enemy->setNeedRemove(true);
            createEffect(enemy_center,enemy->getTag());
        }
        // 玩家跳起效果
        player->getComponent<engine::component::PhysicsComponent>()->velocity_.y = -300.0f;
    }
    // 碰撞敌人
    else {
        spdlog::info("敌人 {} 对玩家 {} 造成了伤害",enemy->getName(),player->getName());
        player->getComponent<game::component::PlayerComponent>()->takeDamage(1);
        // TODO: 其他受伤逻辑
    }
}

void GameScene::PlayerVSItemCollision(engine::object::GameObject *player, engine::object::GameObject *item)
{
    if (item->getName() == "fruit"){
        player->getComponent<engine::component::HealthComponent>()->heal(1);
    } else if(item->getName() == "gem") {
        // TODO:加分
    }
    item->setNeedRemove(true);
    auto item_aabb = item->getComponent<engine::component::ColliderComponent>()->getWorldAABB();
    spdlog::debug("玩家 {} 获得了物品 {}",player->getName(),item->getName());
    createEffect(item_aabb.position + item_aabb.size / 2.0f,item->getTag());
}

void GameScene::createEffect(const glm::vec2 &center_pos, const std::string &tag)
{
    auto effect_obj = std::make_unique<engine::object::GameObject>("effect_" + tag);
    effect_obj->addComponent<engine::component::TransformComponent>(center_pos);


    auto animation = std::make_unique<engine::render::Animation>("effect",false);
    if (tag == "enemy"){
        effect_obj->addComponent<engine::component::SpriteComponent>("assets/textures/FX/enemy-deadth.png",
                                                                context_.getResourceManager(),
                                                                engine::utils::Alignment::CENTER);

        for (auto i = 0; i < 5; ++i){
            animation->addFrame({static_cast<float>(i * 40), 0.0f, 40.0f, 41.0f}, 0.1f);
        }
    } else if (tag == "item"){
        effect_obj->addComponent<engine::component::SpriteComponent>("assets/textures/FX/item-feedback.png",
                                                                context_.getResourceManager(),
                                                                engine::utils::Alignment::CENTER);

        for (auto i = 0; i < 4; ++i){
            animation->addFrame({static_cast<float>(i * 32), 0.0f, 32.0f, 32.0f}, 0.1f);
        }
    } else {
        spdlog::warn("未知特效类型：{}",tag);
        return;
    }

    auto* animation_component = effect_obj->addComponent<engine::component::AnimationComponent>();
    animation_component->addAnimation(std::move(animation));
    animation_component->setOneShotRemoveal(true);
    animation_component->playAnimation("effect");
    safeAddGameObject(std::move(effect_obj));
    spdlog::debug("创建特效 {} 完成",tag);

}

} // namespace game::scene