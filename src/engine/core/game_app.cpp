#include "game_app.h"
#include <SDL3/SDL.h>
#include <spdlog/spdlog.h>

#include "time.h"
#include "config.h"
#include "../resource/resource_manager.h"
#include "../render/camera.h"
#include "../render/renderer.h"
#include "../input/input_manager.h"

namespace engine::core{

GameApp::GameApp() {
    time_ = std::make_unique<Time>();
}

GameApp::~GameApp(){
    if (is_running_){
        spdlog::warn("GameApp::~GameApp() - GameApp is still running, stopping it now ... ");
        close();
    }
}

void GameApp::run(){
    if (!init()){
        spdlog::error("GameApp::run() - Failed to initialize the game app");
        return;
    }

    while (is_running_){
        time_->update();
        float delta_time = time_->getDeltaTime();
        input_manager_->update();

        handleEvents();
        update(delta_time);
        render();

        // spdlog::info("GameApp::run() - Frame time: {}", delta_time);
    }

    close();

}

bool GameApp::init() {
    spdlog::trace("GameApp::init() - Initializing the game app ... ");
    if (!initConfig()) return false;
    if (!initSDL()) return false;
    if (!initTime()) return false;
    if (!initResourceManager()) return false;
    if (!initRenderer()) return false;
    if (!initCamera()) return false;
    if (!initInputManager()) return false;

    // 测试代码
    testResourceManager();

    is_running_ = true;
    spdlog::trace("GameApp::init() - Game app initialized successfully");
    return true;
}

void GameApp::handleEvents(){
    if (input_manager_-> shouldQuit())
    {
        spdlog::trace("GameApp::handleEvents() - Quit event received, stopping the game app ... ");
        is_running_ = false;
        return;
    }

    testInputManager(); // 测试代码
}

void GameApp::update(float /* delta_time */){
    // TODO: Implement update logic
    testCamera();
}

void GameApp::render(){
    // TODO: Implement render logic

    renderer_->clearScreen();
    testRenderer();
    renderer_->present();
}

void GameApp::close(){
    spdlog::trace("GameApp::close() - Closing the game app ... ");
    if (sdl_renderer_ != nullptr){
        SDL_DestroyRenderer(sdl_renderer_);
        sdl_renderer_ = nullptr;
    }

    if (window_ != nullptr){
        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }

    SDL_Quit();
    is_running_ = false;;
}

bool GameApp::initConfig()
{
    try
    {
        config_ = std::make_unique<engine::core::Config>("assets/config.json");
    }
    catch(const std::exception& e)
    {
        spdlog::error("GameApp::initConfig() - Failed to initialize Config: {}", e.what());
        return false;
    }

    spdlog::trace("Config initialized successfully");
    return true;
}

bool GameApp::initSDL()
{
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)){
        spdlog::error("GameApp::init() - Failed to initialize SDL: {}", SDL_GetError());
        return false;
    }

    window_ = SDL_CreateWindow(config_->window_title_.c_str(),config_->window_width_,config_->window_height_,SDL_WINDOW_RESIZABLE);
    if (window_ == nullptr){
        spdlog::error("GameApp::init() - Failed to create window: {}", SDL_GetError());
        return false;
    }

    sdl_renderer_ = SDL_CreateRenderer(window_,nullptr);
    if (sdl_renderer_ == nullptr){
        spdlog::error("GameApp::init() - Failed to create renderer: {}", SDL_GetError());
        return false;
    }

    // 设置 VSync (注意：VSync 开启时，驱动程序会尝试将帧率限制到显示器刷新率，有可能会覆盖我们手动设置的帧率 target_fps)
    int vsync = config_->vsync_enabled_ ? SDL_RENDERER_VSYNC_ADAPTIVE : SDL_RENDERER_VSYNC_DISABLED;
    SDL_SetRenderVSync(sdl_renderer_, vsync);
    spdlog::trace("VSync set to {}", config_->vsync_enabled_);

    // 设置渲染器逻辑分辨率为屏幕分辨率的 1/2，针对像素艺术游戏，可以避免拉伸
    SDL_SetRenderLogicalPresentation(sdl_renderer_, config_->window_width_ / 2, config_->window_height_ / 2,SDL_LOGICAL_PRESENTATION_LETTERBOX);
    spdlog::trace("SDL initialized successfully");
    return true;
}

bool GameApp::initTime(){
    try
    {
        time_ = std::make_unique<Time>();
    }
    catch(const std::exception& e)
    {
        spdlog::error("GameApp::initTime() - Failed to initialize Time: {}", e.what());
        return false;
    }
    time_->setTargetFPS(config_->target_fps_);
    spdlog::trace("Time initialized successfully");
    return true;
}

bool GameApp::initResourceManager(){
    try
    {
        resource_manager_ = std::make_unique<engine::resource::ResourceManager>(sdl_renderer_);
    }
    catch(const std::exception& e)
    {
        spdlog::error("GameApp::initResourceManager() - Failed to initialize ResourceManager: {}", e.what());
        return false;
    }
    spdlog::trace("ResourceManager initialized successfully");
    return true;
}

bool GameApp::initRenderer()
{
    try
    {
        renderer_ = std::make_unique<engine::render::Renderer>(sdl_renderer_,resource_manager_.get());
    }
    catch(const std::exception& e)
    {
        spdlog::error("GameApp::initRenderer() - Failed to initialize Renderer: {}", e.what());
        return false;
    }

    spdlog::trace("Renderer initialized successfully");
    return true;
}

bool GameApp::initCamera()
{
    try
    {
        camera_ = std::make_unique<engine::render::Camera>(glm::vec2(config_->window_width_ / 2,config_->window_height_ / 2));
    }
    catch(const std::exception& e)
    {
        spdlog::error("GameApp::initCamera() - Failed to initialize Camera: {}", e.what());
        return false;
    }

    spdlog::trace("Camera initialized successfully");
    return true;
}

bool GameApp::initInputManager()
{
    try
    {
        input_manager_ = std::make_unique<engine::input::InputManager>(sdl_renderer_, config_.get());
    }
    catch(const std::exception& e)
    {
        spdlog::error("GameApp::initInputManager() - Failed to initialize InputManager: {}", e.what());
        return false;
    }

    spdlog::trace("InputManager initialized successfully");
    return true;

}

void GameApp::testResourceManager(){
    resource_manager_->getTexture("assets/textures/Actors/eagle-attack.png");
    resource_manager_->getFont("assets/fonts/VonwaonBitmap-16px.ttf", 16);
    resource_manager_->getSound("assets/audio/button_click.wav");

    resource_manager_->unloadTexture("assets/textures/Actors/eagle-attack.png");
    resource_manager_->unloadFont("assets/fonts/VonwaonBitmap-16px.ttf", 16);
    resource_manager_->unloadSound("assets/audio/button_click.wav");
}

void GameApp::testRenderer()
{
    engine::render::Sprite sprite_world("assets/textures/Actors/frog.png");
    engine::render::Sprite sprite_ui("assets/textures/UI/buttons/Start1.png");
    engine::render::Sprite sprite_parallax("assets/textures/Layers/back.png");

    static float rotation = 0.0f;
    rotation += 0.01f;

    renderer_->drawParallax(*camera_, sprite_parallax, glm::vec2(100,100), glm::vec2(0.5f,0.5f), glm::bvec2(true,false));
    renderer_->drawSprite(*camera_, sprite_world, glm::vec2(200,200), glm::vec2(1.0f,1.0f), rotation);
    renderer_->drawUISprite(sprite_ui, glm::vec2(100,100));

}

void GameApp::testCamera()
{
    auto key_state = SDL_GetKeyboardState(nullptr);
    if (key_state[SDL_SCANCODE_UP]) camera_->move(glm::vec2(0,-1));
    if (key_state[SDL_SCANCODE_DOWN]) camera_->move(glm::vec2(0,1));
    if (key_state[SDL_SCANCODE_LEFT]) camera_->move(glm::vec2(-1,0));
    if (key_state[SDL_SCANCODE_RIGHT]) camera_->move(glm::vec2(1,0));
}

void GameApp::testInputManager()
{
    std::vector<std::string> actions = {
        "move_up",
        "move_down",
        "move_left",
        "move_right",
        "attack",
        "jump",
        "pause",
        "MouseLeftClick",
        "MouseRightClick",
    };

    for (const auto& action : actions)
    {
        if (input_manager_->isActionPressed(action)) {
            spdlog::info("Action '{}' pressed", action);
        }
        if (input_manager_->isActionReleased(action)) {
            spdlog::info("Action '{}' released", action);
        }
        if (input_manager_->isActionDown(action)) {
            spdlog::info("Action '{}' down", action);
        }
    }
}

} // namespace engine::core

