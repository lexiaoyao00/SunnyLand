#include "audio_component.h"
#include "transform_component.h"
#include "../object/game_object.h"
#include "../audio/audio_player.h"
#include "../render/camera.h"
#include <spdlog/spdlog.h>


namespace engine::component {
AudioComponent::AudioComponent(engine::audio::AudioPlayer *audio_player, engine::render::Camera *camera)
    : audio_player_(audio_player), camera_(camera)
{
    if (!audio_player_ || !camera_) {
        throw std::runtime_error("AudioComponent: Invalid arguments");
    }
}

void AudioComponent::playSound(const std::string &sound_id, int channel, bool use_spatial)
{
    auto sound_path = sound_id_to_path_.find(sound_id) != sound_id_to_path_.end() ? sound_id_to_path_[sound_id] : sound_id;
    if (use_spatial && transform_) {
        // TODO: SDL_Mixer 不支持空间定位，未来更换音频库时可以方便地实现
                // 这里给一个简单的功能：150 像素范围内播放，否则不播放
        auto camera_center = camera_->getPosition() + camera_->getViewportSize() / 2.0f;
        auto object_pos = transform_->getPosition();
        float distance = glm::distance(camera_center, object_pos);
        if (distance > 150) {
            spdlog::debug("AudioComponent::playSound: 音效 '{}' 超出范围，不播放", sound_id);
            return;
        }
        audio_player_->playSound(sound_path, channel);
    } else {    // 不使用空间定位
        audio_player_->playSound(sound_path, channel);
    }

}

void AudioComponent::addSound(const std::string &sound_id, const std::string &path)
{
    if (sound_id_to_path_.find(sound_id) != sound_id_to_path_.end()) {
        spdlog::warn("AudioComponent::addSound: 音效 '{}' 已存在, 覆盖旧路径", sound_id);
    }
    sound_id_to_path_[sound_id] = path;
    spdlog::debug("AudioComponent::addSound: 添加音效 '{}' 到路径 '{}'", sound_id, path);
}

void AudioComponent::init()
{
    if (!owner_){
        spdlog::error("AudioComponent 没有所有者 GameObject");
        return;
    }
    transform_ = owner_->getComponent<engine::component::TransformComponent>();
    if (!transform_) {
        spdlog::warn("AudioComponent 的所有者没有 TransformComponent 组件!无法进行空间定位");
    }
}


}   // namespace engine::component