#include "text_renderer.h"
#include "camera.h"
#include "../resource/resource_manager.h"
#include <SDL3_ttf/SDL_ttf.h>
#include <spdlog/spdlog.h>



namespace engine::render {
    TextRenderer::TextRenderer(SDL_Renderer *sdl_renderer, engine::resource::ResourceManager *resource_manager)
        : sdl_renderer_(sdl_renderer), resource_manager_(resource_manager)
    {
        if (!sdl_renderer_ || !resource_manager_) {
            throw std::runtime_error("TextRenderer: 需要一个有效的 SDL_Renderer 和 ResourceManager");
        }

        if (!TTF_WasInit() && TTF_Init() == false) {
            throw std::runtime_error("初始化 SDL_ttf 失败: " + std::string(SDL_GetError()));
        }

        text_engine_ = TTF_CreateRendererTextEngine(sdl_renderer_);
        if (!text_engine_) {
            throw std::runtime_error("创建 SDL_ttf 渲染器失败: " + std::string(SDL_GetError()));
        }
        spdlog::trace("TextRenderer 初始化成功");
    }

    TextRenderer::~TextRenderer()
    {
        if (text_engine_) {
            close();
        }
    }

    void TextRenderer::close()
    {
        if (text_engine_) {
            TTF_DestroyRendererTextEngine(text_engine_);
            text_engine_ = nullptr;
            spdlog::trace("TTF_TextEngine 销毁");
        }
        TTF_Quit(); // 确保在 ResourceManager 销毁之后调用
    }

    void TextRenderer::drawUIText(const std::string &text, const std::string &font_id, int font_size, const glm::vec2 &position, const SDL_FColor &color)
    {
        TTF_Font* font = resource_manager_->getFont(font_id, font_size);
        if (!font) {
            spdlog::warn("字体加载失败: {} 大小 {}", font_id, font_size);
            return;
        }

        // 创建临时 TTF_Text 对象 （后续可考虑使用缓存优化）
        TTF_Text* temp_text_object = TTF_CreateText(text_engine_, font, text.c_str(), 0);
        if (!temp_text_object) {
            spdlog::error("drawUIText 创建 TTF_Text 对象失败: {}", SDL_GetError());
            return;
        }

        // 先渲染一层黑色文字模拟背景
        TTF_SetTextColorFloat(temp_text_object, 0.0f, 0.0f, 0.0f, 1.0f);
        if (!TTF_DrawRendererText(temp_text_object, position.x + 2, position.y + 2)) {
            spdlog::error("drawUIText 渲染 TTF_Text 对象失败: {}", SDL_GetError());
        }

        // 渲染实际文字
        TTF_SetTextColorFloat(temp_text_object, color.r, color.g, color.b, color.a);
        if (!TTF_DrawRendererText(temp_text_object, position.x, position.y)) {
            spdlog::error("drawUIText 渲染 TTF_Text 对象失败: {}", SDL_GetError());
        }

        // 销毁临时 TTF_Text 对象
        TTF_DestroyText(temp_text_object);
    }

    void TextRenderer::drawText(const Camera &camera, const std::string &text, const std::string &font_id, int font_size, const glm::vec2 &position, const SDL_FColor &color)
    {
        glm::vec2 position_screen = camera.worldToScreen(position);

        drawUIText(text,font_id, font_size, position_screen, color);
    }

    glm::vec2 TextRenderer::getTextSize(const std::string &text, const std::string &font_id, int font_size)
    {
        TTF_Font* font = resource_manager_->getFont(font_id, font_size);
        if (!font) {
            spdlog::warn("字体加载失败: {} 大小 {}", font_id, font_size);
            return glm::vec2(0.0f, 0.0f);
        }

        TTF_Text* temp_text_object = TTF_CreateText(text_engine_, font, text.c_str(), 0);
        if (!temp_text_object) {
            spdlog::error("getTextSize 创建 TTF_Text 对象失败: {}", SDL_GetError());
            return glm::vec2(0.0f, 0.0f);
        }

        int width, height;
        TTF_GetTextSize(temp_text_object, &width, &height);

        TTF_DestroyText(temp_text_object);

        return glm::vec2(static_cast<float>(width), static_cast<float>(height));
    }

} // namespace engine::render