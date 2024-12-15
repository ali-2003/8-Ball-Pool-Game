#include "intro.h"

Intro::Intro()
    : background{}, title{}, subtitle{}, f1{}, f2{}
{
    f1.loadFromFile("./pool_assets/Debrosee-ALPnL.ttf", 80);
    f2.loadFromFile("./pool_assets/Debrosee-ALPnL.ttf", 30);
    title.loadFromRenderedText("8 BALL POOL", f1, SDL_Color{0xFF,0,0,0xFF});
    subtitle.loadFromRenderedText("Start", f2, SDL_Color{0xFF,0xFF,0xFF,0xFF});
    background.loadFromFile("./pool_assets/pool_intro2.jpg");
}

void Intro::handle_events()
{
    SDL_Event e;

    while (SDL_PollEvent(&e) != 0)
    {
        if (e.type == SDL_QUIT)
            GameState::next_state = GameStates::Quit;
        else if (e.type == SDL_KEYDOWN || e.type == SDL_MOUSEBUTTONUP)
            GameState::next_state = GameStates::Level;
    }
}

void Intro::logic() {}

void Intro::render()
{
    mainwindow->clear();
    background.render(0, 0);
    title.render(static_cast<int>((mainwindow->getWidth() - title.getWidth()) / 2),
                 static_cast<int>((mainwindow->getHeight()-520)));
    subtitle.render(static_cast<int>((mainwindow->getWidth() - subtitle.getWidth()) / 2),
                    static_cast<int>((mainwindow->getHeight() -420)));
    mainwindow->update();
}
