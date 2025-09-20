#include "game_app.h"
#include <SDL3/SDL.h>
#include <spdlog/spdlog.h>

#include "time.h"
#include "config.h"
#include "context.h"
#include "../object/game_object.h"
#include "../resource/resource_manager.h"
#include "../render/camera.h"
#include "../render/renderer.h"
#include "../input/input_manager.h"

#include "../component/transform_component.h"
#include "../component/sprite_component.h"

#include "../scene/scene_manager.h"

#include "../physics/physics_engine.h"

#include "../../game/scene/game_scene.h"

namespace engine::core{

engine::object::GameObject game_object("test_game_object");

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
    if (!initPhysicsEngine()) return false;

    if (!initContext()) return false;
    if (!initSceneManager()) return false;


    // TODO:测试代码
    auto scene = std::make_unique<game::scene::GameScene>("GameScene", *context_, *scene_manager_);
    scene_manager_->requestPushScene(std::move(scene));

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

    scene_manager_->handleInput();
}

void GameApp::update(float delta_time){
    // TODO: Implement update logic
    scene_manager_->update(delta_time);
}

void GameApp::render(){
    // TODO: Implement render logic

    renderer_->clearScreen();
    scene_manager_->render();
    renderer_->present();
}

void GameApp::close(){
    spdlog::trace("GameApp::close() - Closing the game app ... ");
    // 先关闭场景管理器，确保所有场景都被清理
    scene_manager_->close();

    // 为了确保正确的销毁顺序，有些智能指针对象也需要手动管理
    resource_manager_.reset();

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

bool GameApp::initPhysicsEngine()
{
    try
    {
        physics_engine_ = std::make_unique<engine::physics::PhysicsEngine>();
    }
    catch(const std::exception& e)
    {
        spdlog::error("GameApp::initPhysicsEngine() - Failed to initialize PhysicsEngine: {}", e.what());
        return false;
    }

    spdlog::trace("PhysicsEngine initialized successfully");
    return true;
}

bool GameApp::initContext()
{
    try
    {
        context_ = std::make_unique<engine::core::Context>(*input_manager_, *renderer_, *camera_, *resource_manager_, *physics_engine_);
    }
    catch(const std::exception& e)
    {
        spdlog::error("GameApp::initContext() - Failed to initialize Context: {}", e.what());
        return false;
    }

    return true;

}

bool GameApp::initSceneManager()
{
    try
    {
        scene_manager_ = std::make_unique<engine::scene::SceneManager>(*context_);
    }
    catch(const std::exception& e)
    {
        spdlog::error("GameApp::initSceneManager() - Failed to initialize SceneManager: {}", e.what());
        return false;
    }

    spdlog::trace("SceneManager initialized successfully");
    return true;
}

} // namespace engine::core

