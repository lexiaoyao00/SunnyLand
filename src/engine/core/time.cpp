#include "time.h"
#include <SDL3/SDL_timer.h>
#include <spdlog/spdlog.h>

namespace engine::core {


Time::Time()
{
    last_time_ = SDL_GetTicksNS();
    frame_start_time_ = last_time_;
    spdlog::trace("Time created.Last time: {}", last_time_);
}

void Time::update()
{
    frame_start_time_ = SDL_GetTicksNS();
    auto current_delta_time = static_cast<double>(frame_start_time_ - last_time_) / 1.0e9;
    if (target_frame_time_ > 0.0){
        limitFrameRate(current_delta_time);
    }else{
        delta_time_ = current_delta_time;
    }

    last_time_ = SDL_GetTicksNS();
}

void Time::limitFrameRate(float current_delta_time)
{
    if (current_delta_time < target_frame_time_){
        double time_to_wait = target_frame_time_ - current_delta_time;
        SDL_DelayNS(static_cast<Uint64>(time_to_wait * 1.0e9));
        delta_time_ = static_cast<double>(SDL_GetTicksNS() - last_time_) / 1.0e9;
    }
}

float Time::getDeltaTime() const {
    return delta_time_ * time_scale_;
}

float Time::getUnscaledDeltaTime() const {
    return delta_time_;
}

float Time::getTimeScale() const {
    return time_scale_;
}

void Time::setTimeScale(float time_scale) {
    time_scale_ = time_scale;
}

int Time::getTargetFPS() const {
    return target_fps_;
}

void Time::setTargetFPS(int target_fps) {
    if (target_fps < 0){
        spdlog::warn("Target FPS cannot be negative. Ignoring value: {}", target_fps);
        return;
    }
    target_fps_ = target_fps;
    target_frame_time_ = 1.0 / static_cast<double>(target_fps);

    spdlog::info("Target FPS set to: {}, target frame time: {}", target_fps, target_frame_time_);
}

} // namespace engine::core
