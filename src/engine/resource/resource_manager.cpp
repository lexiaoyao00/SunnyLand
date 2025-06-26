#include "resource_manager.h"
#include "texture_manager.h"
#include "audio_manager.h"
#include "font_manager.h"
#include <spdlog/spdlog.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <glm/glm.hpp>

namespace engine::resource{

ResourceManager::~ResourceManager() = default;

ResourceManager::ResourceManager(SDL_Renderer *renderer)
{
    texture_manager_ = std::make_unique<TextureManager>(renderer);
    audio_manager_ = std::make_unique<AudioManager>();
    font_manager_ = std::make_unique<FontManager>();

    spdlog::trace("ResourceManager created");
}

void ResourceManager::clear(){
    font_manager_->clearFonts();
    audio_manager_->clearAudios();
    texture_manager_->clearTextures();

    spdlog::trace("ResourceManager cleared");
}

// TextureManager
SDL_Texture *ResourceManager::loadTexture(const std::string &file_path) {
    return texture_manager_->loadTexture(file_path);
}

SDL_Texture *ResourceManager::getTexture(const std::string &file_path) {
    return texture_manager_->getTexture(file_path);
}

glm::vec2 ResourceManager::getTextureSize(const std::string &file_path) {
    return texture_manager_->getTextureSize(file_path);
}

void ResourceManager::unloadTexture(const std::string &file_path) {
    texture_manager_->unloadTexture(file_path);
}

void ResourceManager::clearTextures() {
    texture_manager_->clearTextures();
}

// AudioManager Sound(Chunks)
Mix_Chunk *ResourceManager::loadSound(const std::string &file_path) {
    return audio_manager_->loadSound(file_path);
}

Mix_Chunk *ResourceManager::getSound(const std::string &file_path) {
    return audio_manager_->getSound(file_path);
}

void ResourceManager::unloadSound(const std::string &file_path) {
    audio_manager_->unloadSound(file_path);
}

void ResourceManager::clearSounds() {
    audio_manager_->clearSounds();
}

// AdudioManager Music
Mix_Music *ResourceManager::loadMusic(const std::string &file_path) {
    return audio_manager_->loadMusic(file_path);
}

Mix_Music *ResourceManager::getMusic(const std::string &file_path) {
    return audio_manager_->getMusic(file_path);
}

void ResourceManager::unloadMusic(const std::string &file_path) {
    audio_manager_->unloadMusic(file_path);
}

void ResourceManager::clearMusics() {
    audio_manager_->clearMusics();
}

// FontManager
TTF_Font *ResourceManager::loadFont(const std::string &file_path, int size) {
    return font_manager_->loadFont(file_path, size);
}

TTF_Font *ResourceManager::getFont(const std::string &file_path, int size) {
    return font_manager_->getFont(file_path, size);
}

void ResourceManager::unloadFont(const std::string &file_path, int size) {
    font_manager_->unloadFont(file_path, size);
}

void ResourceManager::clearFonts() {
    font_manager_->clearFonts();
}


}   // namespace engine::resource


