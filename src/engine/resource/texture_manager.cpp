#include "texture_manager.h"
#include <stdexcept>
#include <SDL3_image/SDL_image.h>
#include <spdlog/spdlog.h>

namespace engine::resource {

TextureManager::TextureManager(SDL_Renderer* renderer) : renderer_(renderer) {
    if (!renderer_) {
        throw std::runtime_error("TextureManager constructor: renderer is null");
    }

    spdlog::trace("TextureManager construct success");
}

SDL_Texture* TextureManager::loadTexture(const std::string& path) {
    // 检查是否已经加载过该纹理
    auto it = textures_.find(path);
    if (it != textures_.end()) {
        return it->second.get();
    }

    // 加载纹理
    SDL_Texture* raw_texture = IMG_LoadTexture(renderer_, path.c_str());
    if (!raw_texture) {
        spdlog::error("Failed to load texture: '{}' : {}", path, SDL_GetError());
        return nullptr;
    }

    // 将纹理包装到智能指针中并存储
    textures_.emplace(path, std::unique_ptr<SDL_Texture,SDLTextureDeleter>(raw_texture));
    spdlog::debug("Texture loaded: {}", path);

    return raw_texture;
}

SDL_Texture* TextureManager::getTexture(const std::string& path) {
    auto it = textures_.find(path);
    if (it != textures_.end()) {
        return it->second.get();
    }

    spdlog::warn("Texture not found: {}, trying to load", path);
    return loadTexture(path);
}

glm::vec2 TextureManager::getTextureSize(const std::string& path) {
    SDL_Texture* texture = getTexture(path);
    if (!texture) {
        spdlog::error("Failed to get texture: {}", path);
        return glm::vec2(0, 0);
    }

    glm::vec2 size;
    if (!SDL_GetTextureSize(texture, &size.x, &size.y)) {
        spdlog::error("Failed to get texture size: {}", path);
        return glm::vec2(0, 0);
    }

    return size;
}

void TextureManager::unloadTexture(const std::string& path) {
    auto it = textures_.find(path);
    if (it != textures_.end()) {
        spdlog::debug("Unloading texture: {}", path);
        textures_.erase(it);
    } else {
        spdlog::warn("Texture not found: {}", path);
    }
}

void TextureManager::clearTextures() {
    if (!textures_.empty()) {
        spdlog::debug("Clearing all {} textures.", textures_.size());
        textures_.clear();
    }
}

}   // namespace engine::resource