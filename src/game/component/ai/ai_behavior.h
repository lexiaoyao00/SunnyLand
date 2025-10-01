#pragma once

namespace game::component {
    class AIComponent;
}

namespace game::component::ai {

class AIBehavior{
    friend class game::component::AIComponent;
public:
    AIBehavior() = default;
    virtual ~AIBehavior() = default;

    AIBehavior(const AIBehavior&) = delete;
    AIBehavior& operator=(const AIBehavior&) = delete;
    AIBehavior(AIBehavior&&) = delete;
    AIBehavior& operator=(AIBehavior&&) = delete;

protected:
    // 没有 owner 指针，因此需要传入 AIComponent 引用
    virtual void enter(AIComponent&) {}     // 可选实现
    virtual void update(float, AIComponent&) = 0;   // 必须实现，更新 AI 行为逻辑（具体策略）
};
}   // namespace game::component::ai