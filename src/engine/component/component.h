#pragma once

namespace engine::object
{
    class GameObject;
}

namespace engine::core
{
    class Context;
}

namespace engine::component
{
    class Component
    {
        friend class engine::object::GameObject; // 需要调用 Component 的 init 方法

    protected:
        engine::object::GameObject *owner_ = nullptr; // 指向拥有此组件的GameObject

    public:
        Component() = default;
        virtual ~Component() = default;

        Component(const Component &) = delete;
        Component &operator=(const Component &) = delete;
        Component(Component &&) = delete;
        Component &operator=(Component &&) = delete;

        void setOwner(engine::object::GameObject *owner) { owner_ = owner; }
        engine::object::GameObject *getOwner() const { return owner_; }


    protected:
        virtual void init() {} // 保留两段初始化的机制, GameObject 添加组件时自动调用, 不需要外部调用
        virtual void handleInput(engine::core::Context&) {}
        virtual void update(float, engine::core::Context&) {}
        virtual void render(engine::core::Context&) {}
        virtual void clean() {}
    };
} // namespace engine::component