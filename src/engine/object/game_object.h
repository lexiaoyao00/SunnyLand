#pragma once
#include "../component/component.h"
#include <memory>
#include <unordered_map>
#include <typeindex>    // 用于类型索引
#include <utility>      // 用于完美转发
#include <spdlog/spdlog.h>


namespace engine::object {


class GameObject final {
private:
    std::string name_;
    std::string tag_;
    std::unordered_map<std::type_index, std::unique_ptr<engine::component::Component>> components_;
    bool need_remove_ = false; // 标记是否需要删除


public:

    GameObject(const std::string& name = "", const std::string& tag = "");

    GameObject(const GameObject&) = delete;
    GameObject& operator=(const GameObject&) = delete;
    GameObject(GameObject&&) = delete;
    GameObject& operator=(GameObject&&) = delete;

    void setName(const std::string& name) { name_ = name; }
    const std::string& getName() const { return name_; }
    void setTag(const std::string& tag) { tag_ = tag; }
    const std::string& getTag() const { return tag_; }
    void setNeedRemove(bool need_remove) { need_remove_ = need_remove; }
    bool isNeedRemove() const { return need_remove_; }

    template<typename T, typename... Args>
    T* addComponent(Args&&... args) {
        // 检测组件是否合法     /* static_assert(condition, message): 静态断言，在编译期检测，无任何性能影响 */
                            /* std::is_base_of<Base, Derived>::value -- 判断 Base 类型是否是 Derived 类型的基类 */
        static_assert(std::is_base_of<engine::component::Component, T>::value, "T must be derived from Component");

        auto type_index = std::type_index(typeid(T));
        if (hasComponent<T>()){
            return getComponent<T>();
        }

        auto new_component = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = new_component.get();
        new_component->setOwner(this);
        components_[type_index] = std::move(new_component);
        ptr->init();
        spdlog::debug("GameObject::addComponent: add component {} to game object {}", typeid(T).name(), name_);
        return ptr;
    }

    template<typename T>
    T* getComponent() const {
        static_assert(std::is_base_of<engine::component::Component, T>::value, "T must be derived from Component");

        auto type_index = std::type_index(typeid(T));
        if (auto it = components_.find(type_index); it != components_.end()) {
            return static_cast<T*>(it->second.get());
        }
        spdlog::error("GameObject::getComponent: component {} not found in game object {}", typeid(T).name(), name_);
        return nullptr;
    }

    template<typename T>
    bool hasComponent() const {
        static_assert(std::is_base_of<engine::component::Component, T>::value, "T must be derived from Component");
        return components_.contains(std::type_index(typeid(T)));
    }

    template<typename T>
    void removeComponent() {
        static_assert(std::is_base_of<engine::component::Component, T>::value, "T must be derived from Component");
        auto type_index = std::type_index(typeid(T));
        if (auto it = components_.find(type_index); it != components_.end()) {
            it->second->clean();
            components_.erase(it);
        }
    }

    void update(float delta_time,engine::core::Context& context);
    void render(engine::core::Context& context);
    void clean();
    void handleInput(engine::core::Context& context);
};

}   // namespace engine::object