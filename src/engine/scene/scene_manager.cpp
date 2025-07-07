#include "scene_manager.h"
#include "scene.h"
#include <spdlog/spdlog.h>

namespace engine::scene {

SceneManager::SceneManager(engine::core::Context &context) : context_(context)
{
    spdlog::trace("SceneManager has been created");
}

SceneManager::~SceneManager()
{
    spdlog::trace("SceneManager has been destroyed");
    close();
}

void SceneManager::requestPushScene(std::unique_ptr<Scene> &&scene)
{
    pending_action_ = PendingAction::PUSH;
    pending_scene_ = std::move(scene);
}

void SceneManager::requestPopScene()
{
    pending_action_ = PendingAction::POP;
}

void SceneManager::requestReplaceScene(std::unique_ptr<Scene> &&scene)
{
    pending_action_ = PendingAction::REPLACE;
    pending_scene_ = std::move(scene);
}

Scene *SceneManager::getCurrentScene() const
{
    if (scene_stack_.empty())
    {
        return nullptr;
    }

    return scene_stack_.back().get();
}

void SceneManager::update(float delta_time)
{
    Scene* current_scene = getCurrentScene();
    if (current_scene)
    {
        current_scene->update(delta_time);
    }

    processPendingAction();
}

void SceneManager::render()
{
    for (const auto& scene : scene_stack_)
    {
        if (scene) {
            scene->render();
        }
    }
}
void SceneManager::handleInput()
{
    Scene* current_scene = getCurrentScene();
    if (current_scene)
    {
        current_scene->handleInput();
    }
}

void SceneManager::close()
{
    spdlog::trace("Closing scene manager and cleaning all scenes...");
    while (!scene_stack_.empty())
    {
        if (scene_stack_.back())
        {
            spdlog::debug("Cleaning scene {}", scene_stack_.back()->getName());
            scene_stack_.back()->clean();
        }
        scene_stack_.pop_back();
    }
}

void SceneManager::processPendingAction()
{
    if (pending_action_ == PendingAction::NONE)
    {
        return;
    }

    switch (pending_action_)
    {
    case PendingAction::POP:
        popScene();
        break;
    case PendingAction::PUSH:
        pushScene(std::move(pending_scene_));
        break;
    case PendingAction::REPLACE:
        replaceScene(std::move(pending_scene_));
        break;

    default:
        break;
    }

    pending_action_ = PendingAction::NONE;
}

void SceneManager::pushScene(std::unique_ptr<Scene> &&scene)
{
    if (!scene)
    {
        spdlog::warn("Trying to push null scene to scene stack");
        return;
    }
    spdlog::debug("Pushing scene {} to scene stack", scene->getName());
    if (!scene->isInitialized())
    {
        scene->init();
    }

    scene_stack_.push_back(std::move(scene));

}

void SceneManager::popScene()
{
    if (scene_stack_.empty())
    {
        spdlog::warn("Trying to pop scene from empty scene stack");
        return;
    }

    spdlog::debug("Popping scene {} from scene stack", scene_stack_.back()->getName());
    if (scene_stack_.back())
    {
        scene_stack_.back()->clean();
    }
    scene_stack_.pop_back();
}

void SceneManager::replaceScene(std::unique_ptr<Scene> &&scene)
{
    if (!scene)
    {
        spdlog::warn("Trying to replace scene with null scene");
        return;
    }

    while (!scene_stack_.empty())
    {
        if (scene_stack_.back())
        {
            scene_stack_.back()->clean();
        }
        scene_stack_.pop_back();
    }

    if (!scene->isInitialized())
    {
        scene->init();
    }

    scene_stack_.push_back(std::move(scene));
}

} // namespace engine::scene