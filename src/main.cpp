#include "engine/core/game_app.h"

int main(int /* argc */, char** /* argv[] */)
{
    // Create the game app
    engine::core::GameApp game_app;

    // Run the game app
    game_app.run();

    // Return success
    return 0;
}