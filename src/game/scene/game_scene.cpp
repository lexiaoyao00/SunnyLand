#include "game_scene.h"
#include "../../engine/object/game_object.h"
#include "../../engine/component/transform_component.h"
#include "../../engine/component/sprite_component.h"
#include "../../engine/core/context.h"
#include "../../engine/scene/level_loader.h"
#include "../../engine/input/input_manager.h"
#include "../../engine/render/camera.h"

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
}

void GameScene::handleInput()
{
    // TODO:
    Scene::handleInput();
    testCamera();
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

    test_object->addComponent<engine::component::TransformComponent>(glm::vec2(100.0f,100.0f));
    test_object->addComponent<engine::component::SpriteComponent>("assets/textures/Props/big-crate.png", context_.getResourceManager());

    addGameObject(std::move(test_object));
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
}