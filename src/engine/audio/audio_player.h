#pragma once
#include <string>

namespace engine::resource {
    class ResourceManager;
}

struct Mix_Chuunk;
struct Mix_Music;

namespace engine::audio {

class AudioPlayer final {

private:
    engine::resource::ResourceManager* resource_manager_;   // 指向 ResourceManager 的非拥有指针
    std::string current_music_;   // 当前播放的音乐路径，用于避免重复播放同一音乐

public:
    explicit AudioPlayer(engine::resource::ResourceManager* resource_manager);
    ~AudioPlayer();

    AudioPlayer(const AudioPlayer&) = delete;
    AudioPlayer& operator=(const AudioPlayer&) = delete;
    AudioPlayer(AudioPlayer&&) = delete;
    AudioPlayer& operator=(AudioPlayer&&) = delete;

    // 播放控制方法
    /**
     * @brief 播放音效（chunk）
     *
     * @param sound_path 音效文件路径
     * @param channel 要播放的特定通道，或 -1 表示第一个可用的通道。默认 -1
     * @return int 播放的音效通道，出错时返回 -1
     */
    int playSound(const std::string& sound_path, int channel = -1);

    /**
     * @brief 播放背景音乐（music）。如果正在播放，则淡出之前的音乐
     *
     * @param music_path 音乐文件路径
     * @param loops 循环次数（-1 表示无限循环，0 播放一次，1 播放两次，以此类推）。默认 -1
     * @param fade_in_ms 音乐淡入的时间（毫秒）（0 表示不淡入）。默认 0
     * @return true 成功
     * @return false 出错
     */
    bool playMusic(const std::string& music_path, int loops = -1, int fade_in_ms = 0);

    /**
     * @brief 停止当前正在播放的音乐
     *
     * @param fade_out_ms 淡出时间（毫秒）（0 表示立即停止）。默认 0
     */
    void stopMusic(int fade_out_ms = 0);

    void pauseMusic();  // 暂停当前正在播放的音乐
    void resumeMusic(); // 恢复暂停的音乐

    /**
     * @brief 设置音效通道的音量
     *
     * @param volume 音量级别（0.0 ~ 1.0）
     * @param channel 通道号（-1 表示所有通道）。默认 -1
     */
    void setSoundVolume(float volume, int channel = -1);
    void setMusicVolume(float volume); // 设置音乐的音量
    float getSoundVolume(int channel = -1); // 获取音效通道的音量
    float getMusicVolume(); // 获取音乐的音量


};

}   // namespace engine::audio