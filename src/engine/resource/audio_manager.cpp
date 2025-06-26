#include "audio_manager.h"
#include <spdlog/spdlog.h>

namespace engine::resource {

AudioManager::AudioManager() {
    MIX_InitFlags flags = MIX_INIT_MP3 | MIX_INIT_OGG;
    if ((Mix_Init(flags) & flags) != flags) {
        throw std::runtime_error("SDL_mixer could not initialize! SDL_mixer Error: " + std::string(SDL_GetError()));
    }

    if (!Mix_OpenAudio(0,nullptr)) {
        Mix_Quit();
        throw std::runtime_error("SDL_mixer could not initialize! SDL_mixer Error: " + std::string(SDL_GetError()));
    }

    spdlog::trace("AudioManager created");
}

AudioManager::~AudioManager() {
    Mix_HaltChannel(-1);    // stop all channels
    Mix_HaltMusic();       // stop music

    clearSounds();
    clearMusics();

    Mix_CloseAudio();      // close audio
    Mix_Quit();            // quit SDL_mixer
    spdlog::trace("AudioManager destroyed");
}

// Sounds
Mix_Chunk* AudioManager::loadSound(const std::string& path) {
    auto sound = sounds_.find(path);
    if (sound != sounds_.end()) {
        return sound->second.get();
    }

    spdlog::debug("Loading sound: {}", path);
    Mix_Chunk* raw_chunk = Mix_LoadWAV(path.c_str());
    if (!raw_chunk) {
        spdlog::error("Failed to load sound: {}", path);
        return nullptr;
    }

    sounds_.emplace(path, std::unique_ptr<Mix_Chunk,SDLMixChunkDeleter>(raw_chunk));
    spdlog::debug("Loaded sound: {}", path);
    return raw_chunk;
}

Mix_Chunk* AudioManager::getSound(const std::string& path) {
    auto sound = sounds_.find(path);
    if (sound != sounds_.end()) {
        return sound->second.get();
    }

    spdlog::warn("Sound not found: {}, trying to load", path);
    return loadSound(path);
}

void AudioManager::unloadSound(const std::string& path) {
    auto sound = sounds_.find(path);
    if (sound != sounds_.end()) {
        spdlog::debug("Unloading sound: {}", path);
        sounds_.erase(sound);
    } else {
        spdlog::warn("trying to unload sound that does not exist: {}", path);
    }
}

void AudioManager::clearSounds() {
    if (!sounds_.empty()) {
        spdlog::debug("Clearing all sounds");
        sounds_.clear();
    }
}

// Musics
Mix_Music* AudioManager::loadMusic(const std::string& path) {
    auto music = musics_.find(path);
    if (music != musics_.end()) {
        return music->second.get();
    }

    spdlog::debug("Loading music: {}", path);
    Mix_Music* raw_music = Mix_LoadMUS(path.c_str());
    if (!raw_music) {
        spdlog::error("Failed to load music: {}", path);
        return nullptr;
    }

    musics_.emplace(path, std::unique_ptr<Mix_Music,SDLMixMusicDeleter>(raw_music));
    spdlog::debug("Loaded music: {} .", path);

    return raw_music;
}

Mix_Music* AudioManager::getMusic(const std::string& path) {
    auto music = musics_.find(path);
    if (music != musics_.end()) {
        return music->second.get();
    }

    spdlog::warn("Music not found: {}, trying to load", path);
    return loadMusic(path);
}

void AudioManager::unloadMusic(const std::string& path) {
    auto music = musics_.find(path);
    if (music != musics_.end()) {
        spdlog::debug("Unloading music: {}", path);
        musics_.erase(music);
    } else {
        spdlog::warn("trying to unload music that does not exist: {}", path);
    }
}

void AudioManager::clearMusics() {
    if (!musics_.empty()) {
        spdlog::debug("Clearing all musics");
        musics_.clear();
    }
}

void AudioManager::clearAudios()
{
    clearSounds();
    clearMusics();
}

}   // namespace engine::resource
