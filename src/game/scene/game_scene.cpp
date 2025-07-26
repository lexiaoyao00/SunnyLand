#include "game_scene.h"
#include "../../engine/object/game_object.h"
#include "../../engine/component/transform_component.h"
#include "../../engine/component/sprite_component.h"
#include "../../engine/component/physics_component.h"
#include "../../engine/component/collider_component.h"
#include "../../engine/component/tilelayer_component.h"
#include "../../engine/core/context.h"
#include "../../engine/scene/level_loader.h"
#include "../../engine/input/input_manager.h"
#include "../../engine/render/camera.h"
#include "../../engine/physics/physics_engine.h"

#include <spdlog/spdlog.h>

namespace game::scene {

GameScene::GameScene(std::string name, engine::core::Context &context, engine::scene::SceneManager &scene_manager)
: Scene(name, context, scene_manager)
{
    spdlog::trace("GameScene constructor");
}

void GameScene::init()
{
    engine::scene::LevelLoader level_loader;
    level_loader.loadLevel("assets/maps/level1.tmj", this);

    // 注册 main 注册到物理引擎
    auto* main_layer = findGameObjectByName("main");
    if (main_layer) {
        auto* tile_layer = main_layer->getComponent<engine::component::TileLayerComponent>();
        if (tile_layer) {
            context_.getPhysicsEngine().registerCollisionTileLayer(tile_layer);
            spdlog::info("Collision tile layer has been registered to physics engine");
        }
    }

    // 创建测试对象
    createTestObject();

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
    testCollisionPairs();
}

void GameScene::handleInput()
{
    // TODO:
    Scene::handleInput();
    // testCamera();
    testObject();
}

void GameScene::clean()
{
    // TODO:
    Scene::clean();
    spdlog::trace("GameScene has been cleaned");
}

void GameScene::createTestObject()
{
    spdlog::trace("Creating test object");
    auto test_object = std::make_unique<engine::object::GameObject>("test_object");
    test_object_ = test_object.get();

    test_object->addComponent<engine::component::TransformComponent>(glm::vec2(100.0f,100.0f));
    test_object->addComponent<engine::component::SpriteComponent>("assets/textures/Props/big-crate.png", context_.getResourceManager());
    test_object->addComponent<engine::component::PhysicsComponent>(&context_.getPhysicsEngine());
    test_object->addComponent<engine::component::ColliderComponent>(
        std::make_unique<engine::physics::AABBCollider>(glm::vec2(32.0f, 32.0f))
    );

    addGameObject(std::move(test_object));

    auto test_object2 = std::make_unique<engine::object::GameObject>("test_object2");
    test_object2->addComponent<engine::component::TransformComponent>(glm::vec2(50.0f,50.0f));
    test_object2->addComponent<engine::component::SpriteComponent>("assets/textures/Props/big-crate.png", context_.getResourceManager());
    test_object2->addComponent<engine::component::PhysicsComponent>(&context_.getPhysicsEngine(), false);
    test_object2->addComponent<engine::component::ColliderComponent>(
        std::make_unique<engine::physics::AABBCollider>(glm::vec2(32.0f, 32.0f))
    );

    addGameObject(std::move(test_object2));

    spdlog::trace("Test object has been created in GameScene");
}
void GameScene::testCamera()
{
    auto& camera = context_.getCamera();
    auto& input_manager = context_.getInputManager();
    if (input_manager.isActionDown("move_up")) camera.move(glm::vec2(0.0f, -1.0f));
    if (input_manager.isActionDown("move_down")) camera.move(glm::vec2(0.0f, 1.0f));
    if (input_manager.isActionDown("move_left")) camera.move(glm::vec2(-1.0f, 0.0f));
    if (input_manager.isActionDown("move_right")) camera.move(glm::vec2(1.0f, 0.0f));
}
void GameScene::testObject()
{
    if (!test_object_) return;
    auto& input_manager = context_.getInputManager();
    auto* pc = test_object_->getComponent<engine::component::PhysicsComponent>();
    if (!pc) return;

    if (input_manager.isActionDown("move_left")){
        pc->velocity_.x = -100.0f;
    } else {
        pc->velocity_.x *= 0.9f;
    }

    if (input_manager.isActionDown("move_right")){
        pc->velocity_.x = 100.0f;
    } else {
        pc->velocity_.x *= 0.9f;
    }

    if (input_manager.isActionDown("jump")){
        pc->velocity_.y = -400.0f;
    }

}
void GameScene::testCollisionPairs()
{
    auto clollision_pairs = context_.getPhysicsEngine().getCollisionPairs();
    for (auto& pair : clollision_pairs){
        spdlog::info("Collision pair: {} - {}", pair.first->getName(), pair.second->getName());
    }
}
}