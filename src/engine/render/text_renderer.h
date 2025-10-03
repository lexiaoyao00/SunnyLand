#pragma once
#include <SDL3/SDL_render.h>
#include <string>
#include <glm/vec2.hpp>
#include "../utils/math.h"


struct TTF_TextEngine;

namespace engine::resource {
    class ResourceManager;
}

namespace engine::utils {
    struct FColor;
}


namespace engine::render {
    class Camera;

class TextRenderer final {

private:
    SDL_Renderer* sdl_renderer_ = nullptr;      // 持有渲染器的非拥有指针
    engine::resource::ResourceManager* resource_manager_ = nullptr; // 持有资源管理器的非拥有指针

    TTF_TextEngine* text_engine_ = nullptr; // 使用 SDL3 引入的 TTF_TextEngine 来进行绘制

public:
    TextRenderer(SDL_Renderer* sdl_renderer, engine::resource::ResourceManager* resource_manager);
    ~TextRenderer();

    TextRenderer(const TextRenderer&) = delete;
    TextRenderer& operator=(const TextRenderer&) = delete;
    TextRenderer(TextRenderer&&) = delete;
    TextRenderer& operator=(TextRenderer&&) = delete;


    void close();

    /**
     * @brief 绘制 UI 上的字符串
     *
     * @param text UTF-8 字符串内容
     * @param font_id 字体 id
     * @param font_size 字体大小
     * @param position 左上角屏幕位置
     * @param color 文本颜色（默认为白色）
     */
    void drawUIText(const std::string& text, const std::string& font_id, int font_size,
                    const glm::vec2& position, const engine::utils::FColor& color = {1.0f, 1.0f, 1.0f, 1.0f});


    /**
     * @brief 绘制地图上的字符串
     *
     * @param camera 相机
     * @param text UTF-8 字符串内容
     * @param font_id 字体 id
     * @param font_size 字体大小
     * @param position 左上角屏幕位置
     * @param color 文本颜色（默认为白色）
     */
    void drawText(const Camera& camera, const std::string& text, const std::string& font_id, int font_size,
                const glm::vec2& position, const engine::utils::FColor& color = {1.0f, 1.0f, 1.0f, 1.0f});


    glm::vec2 getTextSize(const std::string& text, const std::string& font_id, int font_size);

};

} // namespace engine::render
