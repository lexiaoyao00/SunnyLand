#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <SDL3/SDL_render.h>
#include <glm/glm.hpp>

namespace engine::resource {

class TextureManager {
    friend class ResourceManager;

private:
    // SDL_Texture 的删除器对象,用于智能指针管理
    struct SDLTextureDeleter {
        void operator()(SDL_Texture* texture) const {
            if (texture) {
                SDL_DestroyTexture(texture);
            }
        }
    };

    std::unordered_map<std::string, std::unique_ptr<SDL_Texture, SDLTextureDeleter>> textures_;
    SDL_Renderer* renderer_ = nullptr;      //指向主渲染器的非拥有指针

public:
    explicit TextureManager(SDL_Renderer* renderer);

    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;
    TextureManager(TextureManager&&) = delete;
    TextureManager& operator=(TextureManager&&) = delete;

private:
    SDL_Texture* loadTexture(const std::string& path);
    SDL_Texture* getTexture(const std::string& path);
    glm::vec2 getTextureSize(const std::string& path);
    void unloadTexture(const std::string& path);
    void clearTextures();

};

}   // namespace engine::resource