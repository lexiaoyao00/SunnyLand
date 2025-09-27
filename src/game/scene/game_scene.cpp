#include "game_scene.h"
#include "../../engine/object/game_object.h"
#include "../../engine/component/transform_component.h"
#include "../../engine/component/sprite_component.h"
#include "../../engine/component/physics_component.h"
#include "../../engine/component/collider_component.h"
#include "../../engine/component/tilelayer_component.h"
#include "../../engine/component/animation_component.h"
#include "../../engine/core/context.h"
#include "../../engine/scene/level_loader.h"
#include "../../engine/input/input_manager.h"
#include "../../engine/render/camera.h"
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
    testHealth();
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

void GameScene::testHealth()
{
    auto input_manager = context_.getInputManager();
    if (input_manager.isActionPressed("attack")){
        player_->getComponent<game::component::PlayerComponent>()->takeDamage(1);
    }
}

} // namespace game::scene