#pragma once
#include <vector>
#include <memory>
#include <string>

namespace engine::core {
    class Context;
}

namespace engine::render {
    class Renderer;
    class Camera;
}

namespace engine::input {
    class InputManager;
}

namespace engine::object {
    class GameObject;
}

namespace engine::ui {
    class UIManager;
}

namespace engine::scene {
class SceneManager;

class Scene{
protected:
    std::string scene_name_;
    engine::core::Context& context_;
    engine::scene::SceneManager& scene_manager_;
    std::unique_ptr<engine::ui::UIManager> ui_manager_;

    bool is_initialized_ = false;
    std::vector<std::unique_ptr<engine::object::GameObject>> game_objects_;         // 场景中的游戏对象
    std::vector<std::unique_ptr<engine::object::GameObject>> pending_additions_;   // 待添加的游戏对象（延时添加）

public:
    Scene(std::string name, engine::core::Context& context, engine::scene::SceneManager& scene_manager);
    virtual ~Scene();   // 析构函数，确保子类正确释放资源；放到cpp文件中实现，避免引用 GameObject 的头文件

    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
    Scene(Scene&&) = delete;
    Scene& operator=(Scene&&) = delete;


    virtual void init();
    virtual void update(float delta_time);
    virtual void render();
    virtual void handleInput();
    virtual void clean();


    virtual void addGameObject(std::unique_ptr<engine::object::GameObject>&& game_object);      // 使用右值传递避免拷贝
    virtual void safeAddGameObject(std::unique_ptr<engine::object::GameObject>&& game_object);
    virtual void removeGameObject(engine::object::GameObject* game_object);
    virtual void safeRemoveGameObject(engine::object::GameObject* game_object);



    engine::object::GameObject* findGameObjectByName(const std::string& name) const;

    // getters and setters
    void setName(const std::string& name) {scene_name_ = name;}
    const std::string& getName() const {return scene_name_;}
    void setInitialized(bool is_initialized) {is_initialized_ = is_initialized;}
    bool isInitialized() const {return is_initialized_;}

    engine::core::Context& getContext() {return context_;}
    engine::scene::SceneManager& getSceneManager() {return scene_manager_;}
    const std::vector<std::unique_ptr<engine::object::GameObject>>& getGameObjects() const {return game_objects_;}
    std::vector<std::unique_ptr<engine::object::GameObject>>& getGameObjects() {return game_objects_;}

private:
    void processPendingAdditions();
};
}   // namespace engine::scene