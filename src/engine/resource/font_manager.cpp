#include "font_manager.h"
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace engine::resource {

FontManager::FontManager() {
    if (!TTF_WasInit() && !TTF_Init()) {
        throw std::runtime_error("Failed to initialize SDL_ttf: " + std::string(SDL_GetError()));
    }

    spdlog::trace("FontManager initialized successfully");
}

FontManager::~FontManager() {
    if (!fonts_.empty()){
        spdlog::debug("FontManager destroyed with loaded fonts");
        clearFonts();
    }

    TTF_Quit();
    spdlog::trace("FontManager destroyed successfully");

}

TTF_Font* FontManager::loadFont(const std::string& path, int size) {
    if (size < 0) {
        spdlog::error("Cannot load font '{}' with size less than 0: {}", path, size);
        return nullptr;
    }

    FontKey key(path, size);

    auto it = fonts_.find(key);
    if (it != fonts_.end()) {
        return it->second.get();
    }

    TTF_Font* raw_font = TTF_OpenFont(path.c_str(), size);
    if (!raw_font) {
        spdlog::error("Failed to load font {}: {}", path, std::string(SDL_GetError()));
        return nullptr;
    }

    fonts_.emplace(key, std::unique_ptr<TTF_Font,SDLFontDeleter>(raw_font));
    spdlog::debug("Font loaded successfully: {}", path);
    return raw_font;
}

TTF_Font* FontManager::getFont(const std::string& path, int size) {
    FontKey key(path, size);
    auto it = fonts_.find(key);
    if (it != fonts_.end()) {
        return it->second.get();
    } else {
        spdlog::warn("Failed to get font '{}': trying to load it", path);
        return loadFont(path, size);
    }
}

void FontManager::unloadFont(const std::string& path, int size) {
    FontKey key(path, size);
    auto it = fonts_.find(key);
    if (it == fonts_.end()) {
        spdlog::warn("Failed to unload font '{}': Font not found", path);
        return;
    } else {
        spdlog::debug("Unloading font: {}", path);
        fonts_.erase(it);
    }
}

void FontManager::clearFonts() {
    if (!fonts_.empty()) {
        fonts_.clear();
        spdlog::debug("All {} fonts unloaded successfully", fonts_.size());
    }
}



}   // namespace engine::resource