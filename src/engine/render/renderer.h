#pragma once
#include "sprite.h"
#include <string>
#include <optional>
#include <glm/glm.hpp>

struct SDL_Renderer;
struct SDL_FRect;

namespace engine::resource {
    class ResourceManager;
}

namespace engine::render {
class Camera;

class Renderer final {
private:
    SDL_Renderer* renderer_ = nullptr;      // 指向 SDL_Renderer 的非拥有指针,由外部创建并管理
    engine::resource::ResourceManager* resource_manager_ = nullptr; // 指向 ResourceManager 的非拥有指针,由外部创建并管理

public:
    Renderer(SDL_Renderer* renderer, engine::resource::ResourceManager* resource_manager);

    void drawSprite(const Camera& camera, const Sprite& sprite, const glm::vec2& position,
                    const glm::vec2& scale = {1.0f, 1.0f}, double angle = 0.0f);


    void drawParallax(const Camera& camera, const Sprite& sprite, const glm::vec2& position,
                    const glm::vec2& scroll_factor, const glm::bvec2& repeat = {true, true}, const glm::vec2& scale = {1.0f, 1.0f});

    void drawUISprite(const Sprite& sprite, const glm::vec2& postion, const std::optional<glm::vec2>& size = std::nullopt);


    void present();         // 更新屏幕,包装 SDL_RenderPresent
    void clearScreen();    // 清空屏幕,包装  SDL_RenderClear

    void setDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a); // 设置绘制颜色,包装 SDL_SetRenderDrawColor
    void setDrawColorFloat(float r, float g, float b, float a); // 设置绘制颜色,包装 SDL_SetRenderDrawColorFloat

    SDL_Renderer* getSDLRenderer() const { return renderer_; }


    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(Renderer&&) = delete;

private:
    std::optional<SDL_FRect> getSpriteSrcRect(const Sprite& sprite);    // 获取精灵的源矩形,用于具体绘制
    bool isRectInViewport(const Camera& camera, const SDL_FRect& rect); // 判断矩形是否在视口内

};

}