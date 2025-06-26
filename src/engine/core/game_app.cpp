#include "game_app.h"
#include <SDL3/SDL.h>
#include <spdlog/spdlog.h>

#include "time.h"
#include "../resource/resource_manager.h"

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

    time_->setTargetFPS(144);   // TODO: Make this configurable
    while (is_running_){
        time_->update();
        float delta_time = time_->getDeltaTime();

        handleEvents();
        update(delta_time);
        render();

        // spdlog::info("GameApp::run() - Frame time: {}", delta_time);
    }

    close();

}

bool GameApp::init() {
    spdlog::trace("GameApp::init() - Initializing the game app ... ");
    if (!initSDL()) return false;
    if (!initTime()) return false;
    if (!initResourceManager()) return false;

    // 测试代码
    testResourceManager();

    is_running_ = true;
    spdlog::trace("GameApp::init() - Game app initialized successfully");
    return true;
}

void GameApp::handleEvents(){
    SDL_Event event;
    while (SDL_PollEvent(&event)){
        if (event.type == SDL_EVENT_QUIT){
            is_running_ = false;
        }
    }
}

void GameApp::update(float /* delta_time */){
    // TODO: Implement update logic
}

void GameApp::render(){
    // TODO: Implement render logic
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

bool GameApp::initSDL()
{
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)){
        spdlog::error("GameApp::init() - Failed to initialize SDL: {}", SDL_GetError());
        return false;
    }

    window_ = SDL_CreateWindow("SunnyLand",1280,720,SDL_WINDOW_RESIZABLE);
    if (window_ == nullptr){
        spdlog::error("GameApp::init() - Failed to create window: {}", SDL_GetError());
        return false;
    }

    sdl_renderer_ = SDL_CreateRenderer(window_,nullptr);
    if (sdl_renderer_ == nullptr){
        spdlog::error("GameApp::init() - Failed to create renderer: {}", SDL_GetError());
        return false;
    }
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

void GameApp::testResourceManager(){
    resource_manager_->getTexture("assets/textures/Actors/eagle-attack.png");
    resource_manager_->getFont("assets/fonts/VonwaonBitmap-16px.ttf", 16);
    resource_manager_->getSound("assets/audio/button_click.wav");

    resource_manager_->unloadTexture("assets/textures/Actors/eagle-attack.png");
    resource_manager_->unloadFont("assets/fonts/VonwaonBitmap-16px.ttf", 16);
    resource_manager_->unloadSound("assets/audio/button_click.wav");
}

} // namespace engine::core

