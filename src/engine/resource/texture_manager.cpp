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

    // 载入纹理时，设置纹理缩放模式为最邻近插值（必不可少，否则 TileLayer 渲染中会出现边缘空隙/模糊）
    // 默认 SDL_SCALEMODE_LINEAR 会进行线性滤波做平滑处理，我们的像素游戏TileLayer 每个图像周围时空白图片，与空白图片之间会有模糊边缘
    if (!SDL_SetTextureScaleMode(raw_texture, SDL_SCALEMODE_NEAREST))
    {
        spdlog::warn("Failed to set texture scale mode to nearest");
    }

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