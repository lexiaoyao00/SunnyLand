#pragma once

#include "ai_behavior.h"

namespace game::component::ai {

class PatrolBehavior final : public AIBehavior {
    friend class game::component::AIComponent;

private:
    float patrol_min_x_ = 0.0f;    // 巡逻范围的左边界
    float patrol_max_x_ = 0.0f;    // 巡逻范围的右边界
    float move_speed_ = 50.0f;    // 移动速度(像素/秒)
    bool move_right_ = false;    // 是否向右移动

public:
    PatrolBehavior(float min_x, float max_x, float speed = 50.0f);
    ~PatrolBehavior() override = default;

    PatrolBehavior(const PatrolBehavior&) = delete;
    PatrolBehavior& operator=(const PatrolBehavior&) = delete;
    PatrolBehavior(PatrolBehavior&&) = delete;
    PatrolBehavior& operator=(PatrolBehavior&&) = delete;

private:
    void enter(AIComponent& ai_component) override;
    void update(float, AIComponent& ai_component) override;
};

}   // namespace game::component::ai