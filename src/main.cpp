#include "mainwindow.h"
#include "gamestate.h"
#include "intro.h"

#include <SDL.h>
#include <memory>

int main()
{
    // MainWindow must be the first object instanciated
    MainWindow win (800, 600, "8 BALL POOL");
    win.setIcon("./pool_assets/game_icon.png");

    std::unique_ptr<GameState> currentState {std::make_unique<Intro>()};

    while (GameState::curr_state != GameStates::Quit)
    {
        currentState->handle_events();
        currentState->logic();
        currentState->render();

        changeState(currentState);
    }

    return 0;
}
