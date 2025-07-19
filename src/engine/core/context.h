#pragma once

namespace engine::input
{
    class InputManager;
} // namespace engine::input

namespace engine::render
{
    class Renderer;
    class Camera;
} // namespace engine::render

namespace engine::resource
{
    class ResourceManager;
} // namespace engine::resource

namespace engine::physics
{
    class PhysicsEngine;
}

namespace engine::core
{

    class Context final
    {
    private:
        // 使用引用，确保每个模块都有效，使用时不需要检查指针是否为空
        engine::input::InputManager &input_manager_;
        engine::render::Renderer &renderer_;
        engine::render::Camera &camera_;
        engine::resource::ResourceManager &resource_manager_;
        engine::physics::PhysicsEngine &physics_engine_;


    public:
        Context(
            engine::input::InputManager &input_manager,
            engine::render::Renderer &renderer,
            engine::render::Camera &camera,
            engine::resource::ResourceManager &resource_manager,
            engine::physics::PhysicsEngine &physics_engine);

        Context(const Context &) = delete;
        Context &operator=(const Context &) = delete;
        Context(Context &&) = delete;
        Context &operator=(Context &&) = delete;

        // getters
        engine::input::InputManager &getInputManager() const { return input_manager_; }
        engine::render::Renderer &getRenderer() const { return renderer_; }
        engine::render::Camera &getCamera() const { return camera_; }
        engine::resource::ResourceManager &getResourceManager() const { return resource_manager_; }
        engine::physics::PhysicsEngine &getPhysicsEngine() const { return physics_engine_; }
    };
} // namespace engine::core