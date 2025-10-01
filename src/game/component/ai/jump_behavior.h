#pragma once
#include "ai_behavior.h"
#include <glm/vec2.hpp>

namespace game::component::ai {

class JumpBehavior final : public AIBehavior {
    friend class game::component::AIComponent;
private:
    float patrol_min_x_ = 0.0f; //巡逻范围的左边界
    float patrol_max_x_ = 0.0f; //巡逻范围的右边界
    glm::vec2 jump_vel_ = glm::vec2(100.0f, -300.0f); //跳跃速度
    float jump_interval_ = 2.0f; //跳跃间隔时间（秒）
    float jump_timer_ = 0.0f; //距离下次跳跃的计时器
    bool jumping_right_ = false; //是否向右跳跃

public:
    JumpBehavior(float min_x, float max_x, glm::vec2 jump_vel = glm::vec2(100.0f, -300.0f), float jump_interval = 2.0f);
    ~JumpBehavior() override = default;

    JumpBehavior(const JumpBehavior&) = delete;
    JumpBehavior& operator=(const JumpBehavior&) = delete;
    JumpBehavior(JumpBehavior&&) = delete;
    JumpBehavior& operator=(JumpBehavior&&) = delete;

private:
    void update(float delta_time, AIComponent& ai_component) override;
};

}   // namespace game::component::ai