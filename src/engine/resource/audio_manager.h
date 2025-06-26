#pragma once

#include <memory>
#include <unordered_map>
#include <string>

#include <SDL3_mixer/SDL_mixer.h>

namespace engine::resource {

class AudioManager {
    friend class ResourceManager;

private:
    struct SDLMixChunkDeleter {
        void operator()(Mix_Chunk* chunk) const {
            if (chunk != nullptr) {
                Mix_FreeChunk(chunk);
            }
        }
    };

    struct SDLMixMusicDeleter {
        void operator()(Mix_Music* music) const {
            if (music != nullptr) {
                Mix_FreeMusic(music);
            }
        }
    };

    std::unordered_map<std::string, std::unique_ptr<Mix_Chunk, SDLMixChunkDeleter>> sounds_;
    std::unordered_map<std::string, std::unique_ptr<Mix_Music, SDLMixMusicDeleter>> musics_;

public:
    AudioManager();
    ~AudioManager();

    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;
    AudioManager(AudioManager&&) = delete;
    AudioManager& operator=(AudioManager&&) = delete;

private:
    // Sounds(Chunks)
    Mix_Chunk* loadSound(const std::string& path);
    Mix_Chunk* getSound(const std::string& path);
    void unloadSound(const std::string& path);
    void clearSounds();

    // Musics
    Mix_Music* loadMusic(const std::string& path);
    Mix_Music* getMusic(const std::string& path);
    void unloadMusic(const std::string& path);
    void clearMusics();

    void clearAudios();

};

}   // namespace engine::resource