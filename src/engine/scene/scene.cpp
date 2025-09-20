#include "scene.h"
#include "scene_manager.h"
#include "../core/context.h"
#include "../object/game_object.h"
#include "../physics/physics_engine.h"
#include "../render/camera.h"
#include <spdlog/spdlog.h>

namespace engine::scene {

Scene::Scene(std::string name, engine::core::Context &context, engine::scene::SceneManager &scene_manager)
: scene_name_(name), context_(context), scene_manager_(scene_manager), is_initialized_(false)
{
    spdlog::trace("Scene {} created", scene_name_);
}

Scene::~Scene() = default;

// 子类应该最后调用父类的 init 方法
void Scene::init()
{
    is_initialized_ = true;
    spdlog::trace("Scene {} initialized", scene_name_);
}

void Scene::update(float delta_time)
{
    if (!is_initialized_) return;

    // 先更新物理引擎
    context_.getPhysicsEngine().update(delta_time);
    // 更新相机
    context_.getCamera().update(delta_time);

    for (auto it = game_objects_.begin(); it != game_objects_.end();)
    {
        if (*it && !(*it)->isNeedRemove())
        {
            (*it)->update(delta_time, context_);
            ++it;
        }
        else
        {
            if (*it) {
                (*it)->clean();
            }
            it = game_objects_.erase(it);
        }
    }

    processPendingAdditions();
}

void Scene::render()
{
    if (!is_initialized_) return;

    for (auto &game_object : game_objects_)
    {
            game_object->render(context_);
    }
}

void Scene::handleInput()
{
    if (!is_initialized_) return;

    for (auto it = game_objects_.begin(); it != game_objects_.end();)
    {
        if (*it && !(*it)->isNeedRemove())
        {
            (*it)->handleInput(context_);
            ++it;
        }
        else
        {
            if (*it) {
                (*it)->clean();
            }
            it = game_objects_.erase(it);
        }
    }
}

void Scene::clean()
{
    if (!is_initialized_) return;

    for (auto &game_object : game_objects_)
    {
        game_object->clean();
    }
    game_objects_.clear();

    is_initialized_ = false;
    spdlog::trace("Scene {} cleaned", scene_name_);
}

void engine::scene::Scene::addGameObject(std::unique_ptr<engine::object::GameObject> &&game_object)
{
    if (game_object)
    {
        game_objects_.push_back(std::move(game_object));
    }
    else
    {
        spdlog::warn("Try to add null game object to scene {}", scene_name_);
    }
}
void Scene::safeAddGameObject(std::unique_ptr<engine::object::GameObject> &&game_object)
{
    if (game_object)
    {
        pending_additions_.push_back(std::move(game_object));
    }
    else
    {
        spdlog::warn("Try to add null game object to scene {}", scene_name_);
    }
}
void Scene::removeGameObject(engine::object::GameObject *game_object)
{
    if (!game_object)
    {
        spdlog::warn("Try to remove null game object from scene {}", scene_name_);
        return;
    }

    auto it = std::remove_if(game_objects_.begin(), game_objects_.end(),
        [game_object](const std::unique_ptr<engine::object::GameObject> &obj) {
            return obj.get() == game_object;
        });

    if (it != game_objects_.end())
    {
        (*it)->clean();
        game_objects_.erase(it, game_objects_.end());
        spdlog::trace("Game object {} removed from scene {}", game_object->getName(), scene_name_);
    }
    else
    {
        spdlog::warn("Game object {} not found in scene {}", game_object->getName(), scene_name_);
    }
}

void Scene::safeRemoveGameObject(engine::object::GameObject *game_object)
{
    game_object->setNeedRemove(true);
}

engine::object::GameObject *Scene::findGameObjectByName(const std::string &name) const
{
    // 找到第一个符合条件的游戏对象就返回
    for (const auto &game_object : game_objects_){
        if (game_object && game_object->getName() == name){
            return game_object.get();
        }
    }

    return nullptr;
}

void Scene::processPendingAdditions()
{
    for (auto &game_object : pending_additions_)
    {
        addGameObject(std::move(game_object));
    }
    pending_additions_.clear();
}
} // namespace engine::scene