#include "game_object.h"
#include "../render/renderer.h"
#include "../input/input_manager.h"
#include "../render/camera.h"
#include <spdlog/spdlog.h>

namespace engine::object {

    GameObject::GameObject(const std::string &name, const std::string &tag): name_(name), tag_(tag)
    {
        spdlog::trace("GameObject created: {} {}", name_, tag_);
    }

    void GameObject::update(float delta_time, engine::core::Context& context)
    {
        for (auto &component_pair : components_)
        {
            component_pair.second->update(delta_time,context);
        }
    }

    void GameObject::render(engine::core::Context& context)
    {
        for (auto &component_pair : components_)
        {
            component_pair.second->render(context);
        }
    }

    void GameObject::clean()
    {
        for (auto &component_pair : components_)
        {
            component_pair.second->clean();
        }
        components_.clear();
        spdlog::trace("GameObject cleaned: {} {}", name_, tag_);
    }

    void GameObject::handleInput(engine::core::Context& context)
    {
        for (auto &component_pair : components_)
        {
            component_pair.second->handleInput(context);
        }
    }
} // namespace engine::object