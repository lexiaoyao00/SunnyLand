#pragma once

#include "ai_behavior.h"

namespace game::component::ai {
class UpDownBehavior final : public AIBehavior {
    friend class game::component::AIComponent;

private:
    float patrol_min_y_ = 0.0f;    // 巡逻范围的上边界
    float patrol_max_y_ = 0.0f;    // 巡逻范围的下边界
    float move_speed_ = 50.0f;    // 移动速度(像素/秒)
    bool move_down_ = false;    // 是否向下移动

public:
    UpDownBehavior(float min_y, float max_y, float speed = 50.0f);
    ~UpDownBehavior() override = default;

    UpDownBehavior(const UpDownBehavior&) = delete;
    UpDownBehavior& operator=(const UpDownBehavior&) = delete;
    UpDownBehavior(UpDownBehavior&&) = delete;
    UpDownBehavior& operator=(UpDownBehavior&&) = delete;

private:
    void enter(AIComponent& ai_component) override;
    void update(float, AIComponent& ai_component) override;
};

}   // namespace game::component::ai