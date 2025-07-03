#include "engine/core/game_app.h"
#include <spdlog/spdlog.h>

int main(int /* argc */, char** /* argv[] */)
{
    spdlog::set_level(spdlog::level::debug);
    // Create the game app
    engine::core::GameApp game_app;

    // Run the game app
    game_app.run();

    // Return success
    return 0;
}