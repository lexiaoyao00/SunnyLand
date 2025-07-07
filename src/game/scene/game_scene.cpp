#include "game_scene.h"
#include "../../engine/object/game_object.h"
#include "../../engine/component/transform_component.h"
#include "../../engine/component/sprite_component.h"
#include "../../engine/core/context.h"

#include <spdlog/spdlog.h>

namespace game::scene {

GameScene::GameScene(std::string name, engine::core::Context &context, engine::scene::SceneManager &scene_manager)
: Scene(name, context, scene_manager)
{
    spdlog::trace("GameScene constructor");
}

void GameScene::init()
{
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
}