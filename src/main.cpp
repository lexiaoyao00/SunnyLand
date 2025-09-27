#include "engine/core/game_app.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <windows.h>

int main(int /* argc */, char** /* argv[] */)
{
    // 避免中文乱码
    SetConsoleOutputCP(CP_UTF8);

    spdlog::set_level(spdlog::level::debug);
    // Create the game app
    engine::core::GameApp game_app;

    // Run the game app
    game_app.run();

    // Return success
    return 0;
}