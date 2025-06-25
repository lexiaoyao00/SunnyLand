#pragma once
#include <memory>

struct SDL_Window;
struct SDL_Renderer;

namespace engine::core {
class Time;

class GameApp final {
private:
    SDL_Window *window_ = nullptr;
    SDL_Renderer *sdl_renderer_ = nullptr;
    bool is_running_ = false;

    // engine::core
    std::unique_ptr<engine::core::Time> time_;

public:
    GameApp();
    ~GameApp();

    void run();

    // 禁止拷贝和移动
    GameApp(const GameApp &) = delete;
    GameApp &operator=(const GameApp &) = delete;
    GameApp(GameApp &&) = delete;
    GameApp &operator=(GameApp &&) = delete;

private:
    [[nodiscard]] bool init();  // nodiscard 表示该函数的返回值不应该被忽略
    void handleEvents();
    void update(float delta_time);
    void render();
    void close();

};

}   // namespace engine::core