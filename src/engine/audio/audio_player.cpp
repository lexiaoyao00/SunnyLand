#include "audio_player.h"
#include "../resource/resource_manager.h"
#include <SDL3_mixer/SDL_mixer.h>
#include <spdlog/spdlog.h>
#include <glm/common.hpp>

namespace engine::audio {
AudioPlayer::AudioPlayer(engine::resource::ResourceManager *resource_manager)
    : resource_manager_(resource_manager)
{
    if (! resource_manager_){
        throw std::runtime_error("AudioPlayer 构造失败：提供的 ResourceManager 为空");
    }
}

AudioPlayer::~AudioPlayer() = default;

int AudioPlayer::playSound(const std::string &sound_path, int channel)
{
    Mix_Chunk* chunk = resource_manager_->getSound(sound_path);
    if (! chunk){
        spdlog::error("播放音频失败：找不到音频资源 {}", sound_path);
        return -1;
    }

    int play_channel = Mix_PlayChannel(channel, chunk, 0);  // 播放音效
    if (play_channel == -1){
        spdlog::error("无法播放音效 '{}' ：{}",sound_path, SDL_GetError());
    } else {
        spdlog::trace("播放音效 '{}' 在通道 {}", sound_path, play_channel);
    }
    return play_channel;
}

bool AudioPlayer::playMusic(const std::string &music_path, int loops, int fade_in_ms)
{
    if (music_path == current_music_) return true;

    current_music_ = music_path;
    Mix_Music* music = resource_manager_->getMusic(music_path);
    if (! music){
        spdlog::error("播放音乐失败：找不到音乐资源 {}", music_path);
        return false;
    }
    Mix_HaltMusic();  // 停止当前音乐

    bool result = false;
    if (fade_in_ms > 0){
        result = Mix_FadeInMusic(music, loops, fade_in_ms); // 淡入播放音乐
    } else {
        result = Mix_PlayMusic(music, loops);  // 立即播放音乐
    }

    if (! result){
        spdlog::error("无法播放音乐 '{}' ：{}", music_path, SDL_GetError());
    } else {
        spdlog::trace("播放音乐 '{}' ，循环 {} 次，淡入时间 {} ms", music_path, loops, fade_in_ms);
    }
    return result;
}

void AudioPlayer::stopMusic(int fade_out_ms)
{
    if (fade_out_ms > 0){
        Mix_FadeOutMusic(fade_out_ms);  // 淡出停止音乐
    } else {
        Mix_HaltMusic();  // 立即停止音乐
    }
    // current_music_ = "";
    spdlog::trace("停止音乐，淡出时间 {} ms", fade_out_ms);
}

void AudioPlayer::pauseMusic()
{
    Mix_PauseMusic();
    spdlog::trace("暂停音乐");
}

void AudioPlayer::resumeMusic()
{
    Mix_ResumeMusic();
    spdlog::trace("恢复音乐");
}

void AudioPlayer::setSoundVolume(float volume, int channel)
{
    int sdl_volume = static_cast<int>(glm::max(0.0f, glm::min(1.0f, volume)) * MIX_MAX_VOLUME);
    Mix_Volume(channel, sdl_volume);
    spdlog::trace("设置音效音量：{}，通道 {}", volume, channel);
}

void AudioPlayer::setMusicVolume(float volume)
{
    int sdl_volume = static_cast<int>(glm::max(0.0f, glm::min(1.0f, volume)) * MIX_MAX_VOLUME);
    Mix_VolumeMusic(sdl_volume);
    spdlog::trace("设置音乐音量：{}", volume);
}

float AudioPlayer::getSoundVolume(int channel)
{
    return static_cast<float>(Mix_Volume(channel, -1)) / static_cast<float>(MIX_MAX_VOLUME);
}

float AudioPlayer::getMusicVolume()
{
    return static_cast<float>(Mix_VolumeMusic(-1)) / static_cast<float>(MIX_MAX_VOLUME);
}

}   // namespace engine::audio