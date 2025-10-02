#pragma once

#include "component.h"
#include <unordered_map>
#include <string>

namespace engine::audio {
    class AudioPlayer;
}
namespace engine::render {
    class Camera;
}

namespace engine::component {
    class TransformComponent;

class AudioComponent final: public Component {
    friend class engine::object::GameObject;

private:
    engine::audio::AudioPlayer* audio_player_;  // 音频播放器的非拥有指针
    engine::render::Camera* camera_;  // 相机的非拥有指针，用于音频空间定位
    engine::component::TransformComponent* transform_;  // 位置变换组件的非拥有指针

    std::unordered_map<std::string, std::string> sound_id_to_path_; // 音效ID到路径的映射

public:
    AudioComponent(engine::audio::AudioPlayer* audio_player, engine::render::Camera* camera);
    ~AudioComponent() override = default;

    AudioComponent(const AudioComponent&) = delete;
    AudioComponent& operator=(const AudioComponent&) = delete;
    AudioComponent(AudioComponent&&) = delete;
    AudioComponent& operator=(AudioComponent&&) = delete;

    /**
     * @brief 播放音效
     *
     * @param sound_id 音效文件的id（或路径）。
     * @param channel 要播放的特定通道哦，或 -1 表示第一个可用通道
     * @param use_spatial 是否使用空间定位
     */
    void playSound(const std::string& sound_id, int channel = -1, bool use_spatial = false);

    void addSound(const std::string& sound_id, const std::string& path);

private:
    void init() override;
    void update(float, engine::core::Context&) override {}

};

}   // namespace engine::component