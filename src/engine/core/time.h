#pragma once
#include <SDL3/SDL_stdinc.h>

namespace engine::core {

class Time final{
private:
    Uint64 last_time_ = 0;  // last frame time
    Uint64 frame_start_time_ = 0;   // current frame start time
    double delta_time_ = 0;  // time between current and last frame
    double time_scale_ = 1.0;  // time scale

    int target_fps_ = 0;  // target fps
    double target_frame_time_ = 0.0;  // target time between frames

public:
    Time();

    Time(const Time&) = delete;
    Time& operator=(const Time&) = delete;
    Time(Time&&) = delete;
    Time& operator=(Time&&) = delete;

    void update();

    float getDeltaTime() const;
    float getUnscaledDeltaTime() const;

    float getTimeScale() const;
    void setTimeScale(float time_scale);
    int getTargetFPS() const;
    void setTargetFPS(int target_fps);

private:
    void limitFrameRate(float current_delta_time);
};
}