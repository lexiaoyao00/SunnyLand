#pragma once
#include <memory>
#include <string>
#include <glm/glm.hpp>

struct SDL_Renderer;
struct SDL_Texture;
struct Mix_Music;
struct Mix_Chunk;
struct TTF_Font;

namespace engine::resource {

class TextureManager;
class AudioManager;
class FontManager;

class ResourceManager {
private:
    std::unique_ptr<TextureManager> texture_manager_;
    std::unique_ptr<AudioManager> audio_manager_;
    std::unique_ptr<FontManager> font_manager_;
public:
    explicit ResourceManager(SDL_Renderer* renderer);
    ~ResourceManager();     // 显式声明析构函数,为了让智能指针正确管理仅有前向声明的类

    void clear();

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    ResourceManager(ResourceManager&&) = delete;
    ResourceManager& operator=(ResourceManager&&) = delete;

    // TextureManager
    SDL_Texture* loadTexture(const std::string& file_path);     // 加载纹理文件
    SDL_Texture* getTexture(const std::string& file_path);      // 尝试获取已经加载的纹理,没有则尝试加载
    void unloadTexture(const std::string& file_path);            // 卸载纹理文件
    glm::vec2 getTextureSize(const std::string& file_path);     // 获取纹理尺寸
    void clearTextures();                                      // 清空所有纹理

    // AudioManager Sound(Chunks)
    Mix_Chunk* loadSound(const std::string& file_path);          // 加载音效文件
    Mix_Chunk* getSound(const std::string& file_path);           // 尝试获取已经加载的音效,没有则尝试加载
    void unloadSound(const std::string& file_path);              // 卸载音效文件
    void clearSounds();                                         // 清空所有音效

    // AudioManager Music
    Mix_Music* loadMusic(const std::string& file_path);          // 加载音乐文件
    Mix_Music* getMusic(const std::string& file_path);           // 尝试获取已经加载的音乐,没有则尝试加载
    void unloadMusic(const std::string& file_path);              // 卸载音乐文件
    void clearMusics();                                          // 清空所有音乐

    // FontManager
    TTF_Font* loadFont(const std::string& file_path, int size);  // 加载字体文件
    TTF_Font* getFont(const std::string& file_path, int size);   // 尝试获取已经加载的字体,没有则尝试加载
    void unloadFont(const std::string& file_path, int size);     // 卸载字体文件
    void clearFonts();                                          // 清空所有字体
};

}   // namespace engine::resource