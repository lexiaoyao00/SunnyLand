#pragma once
#include <memory>
#include <string>
#include <vector>

namespace engine::core {
    class Context;
}

namespace engine::scene {
class Scene;

class SceneManager final {

private:
    engine::core::Context& context_;
    std::vector<std::unique_ptr<Scene>> scene_stack_;              // 场景栈

    enum class PendingAction {
        NONE,
        PUSH,
        POP,
        REPLACE
    };
    PendingAction pending_action_ = PendingAction::NONE;    // 待处理动作
    std::unique_ptr<Scene> pending_scene_;                  // 待处理场景

public:
    explicit SceneManager(engine::core::Context& context);
    ~SceneManager();

    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;
    SceneManager(SceneManager&&) = delete;
    SceneManager& operator=(SceneManager&&) = delete;

    // 延时切换场景，在当前帧结束后切换
    void requestPushScene(std::unique_ptr<Scene>&& scene);      // 请求压入一个新场景
    void requestPopScene();                                     // 请求弹出当前场景
    void requestReplaceScene(std::unique_ptr<Scene>&& scene);   // 请求替换当前场景

    Scene* getCurrentScene() const;                                     // 获取当前场景
    engine::core::Context& getContext() const {return context_;}        // 获取上下文

    void update(float delta_time);
    void render();
    void handleInput();
    void close();

private:
    void processPendingAction();        // 处理挂起的场景操作 (每轮更新最后调用)
    void pushScene(std::unique_ptr<Scene>&& scene);     // 压入一个新场景，使其成为活动场景
    void popScene();                   // 弹出当前场景
    void replaceScene(std::unique_ptr<Scene>&& scene);  // 清理场景栈，压入新场景



};
}   // namespace engine::scene