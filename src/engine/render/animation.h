#pragma once

#include <string>
#include <vector>
#include <SDL3/SDL_rect.h>


namespace engine::render {

struct AnimationFrame
{
    SDL_FRect source_rect;  // 纹理图集上此帧的位置和大小
    float duration;         // 此帧的持续时间（秒）
    /* data */
};

class Animation final{

private:
    std::string name_;      // 动画名称
    std::vector<AnimationFrame> frames_; // 动画帧列表
    float total_duration_ = 0.0f; // 动画总持续时间（秒）
    bool loop_ = true;

public:
    Animation(const std::string& name = "default", bool loop = true);
    ~Animation() = default;

    Animation(const Animation&) = delete;
    Animation& operator=(const Animation&) = delete;
    Animation(Animation&&) = delete;
    Animation& operator=(Animation&&) = delete;

    void addFrame(const SDL_FRect& source_rect, float duration);

    /**
     * @brief 获取在给定事件点应该显示的动画帧
     *
     * @param time 指定时间点，如果动画循环则可以超过总持续时间
     * @return const AnimationFrame& 对应时间点的动画帧
     */
    const AnimationFrame& getFrame(float time) const;

    // setters and getters
    const std::string& getName() const { return name_; }
    const std::vector<AnimationFrame>& getFrames() const { return frames_; }
    size_t getFrameCount() const { return frames_.size(); }
    float getTotalDuration() const { return total_duration_; }
    bool isLooping() const { return loop_; }
    bool isEmpty() const { return frames_.empty(); }

    void setName(const std::string& name) { name_ = name; }
    void setLooping(bool loop) { loop_ = loop; }

};



} // namespace engine::render


