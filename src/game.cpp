#include <iostream>
#include "game.h"
#include "mainwindow.h"

#include <algorithm>
#include <cmath>
#include <string>

#include <SDL.h>

const double PI = std::atan(1) * 4;

Game::Game()
    : background{},collobserver{}, audio{}, tab{&collobserver}
{
    f3.loadFromFile("./pool_assets/Debrosee-ALPnL.ttf", 30);
    score.loadFromRenderedText("hello", f3 ,SDL_Color{0xFF,0,0,0xFF} );
    background.loadFromFile("./pool_assets/pool_intro2.jpg");
    scoreBoard.loadFromFile("./pool_assets/scoreBoard.png");
    
    create_balls();
    create_cue_ball();
}

void Game::handle_events()
{
    SDL_Event e;

    while (SDL_PollEvent(&e) != 0)
    {
        if (e.type == SDL_QUIT)
            GameState::next_state = GameStates::Quit;
        else if (!moving_state)
            handle_when_still(e);
    }
}

void Game::logic()
{
    moving_state = false;

    for (auto& b: balls)
        if (b.is_moving())
        {
            moving_state = true;
            b.move();
            check_pocket(b);
        }

    if (cueball.is_moving())
    {
        moving_state = true;
        cueball.move();
        check_pocket(cueball);
    }


    if (!moving_state)
    {
        if (!mouse_pressed)
            recenter_cue();

        if (move_was_made)
            change_state();
    }
}

void Game::render()
{
    mainwindow->clear();
    // score.render(300,50);
    background.render(0, 0);
    // scoreBoard.render(300,15);
    
    tab.render();

    for (auto& b: balls)
        if (b.is_visible)
            b.render();

    if (cueball.is_visible)
        cueball.render();

    if (!moving_state && cueball.is_visible)
        cue.render();

    mainwindow->update();
}

void Game::handle_when_still(SDL_Event& e)
{
    if (e.type == SDL_KEYDOWN && (e.key.keysym.sym == SDLK_SPACE || e.key.keysym.sym == SDLK_RETURN))
    {
        shoot(11.0);
    }
    else if (e.type == SDL_MOUSEBUTTONDOWN)
    {
        mouse_pressed = true;
    }
    else if (e.type == SDL_MOUSEBUTTONUP)
    {
        double force = std::hypot(cueball.posData.pos_x - cue.getX(),
                                  cueball.posData.pos_y - cue.getY());

        force /= 8.0;

        if (force > 12.0)
            force = 12.0;
        else if (force < 1.0)
            force = 1.0;

        shoot(force);
    }
    else if (e.type == SDL_MOUSEMOTION)
    {
        //Get mouse position
        int x, y;
        SDL_GetMouseState( &x, &y );

        auto oposed = cueball.posData.pos_y - y;
        auto hyp = std::hypot(cueball.posData.pos_x - x, cueball.posData.pos_y - y);
        double degrees = (std::asin(oposed / hyp) * 180.0) / PI + 180.0;

        if (x > cueball.posData.pos_x)
            degrees = 180.0 - degrees;

        if (degrees < 0.0)
            degrees += 360.0;

        if (!mouse_pressed)
            cue.setDeg(degrees);
        else
        {
            double angle = (cue.getAngle() * PI) / 180.0;

            double px = cueball.posData.pos_x + (std::cos(angle) * hyp) + cueball.posData.radius;
            double py = cueball.posData.pos_y + (std::sin(angle) * hyp) - cueball.posData.radius + (cue.getHeight()/2.0);

            cue.setPos(px, py);
        }
    }
}

void Game::create_balls()
{
    // Create 15 balls
    for (int i = 1; i <= 15; ++i)
    {
        balls[i-1].setTex("./pool_assets/ball" + std::to_string(i) + ".png");
        balls[i-1].id = i;
        balls[i-1].addObserver(&collobserver);
        balls[i-1].addObserver(&audio);
    }

    // Place the balls
    int cur = 0;
    double radius = balls[0].posData.radius + 0.5;
    double px = 200.0;
    double py = (tab.getY() + tab.getHeight()/2) - 5*radius;
    for (int i = 5; i > 0; --i)
    {
        for (int j = 0; j < i; ++j)
            balls[cur++].setPos(px, py + radius*j*2);

        px += radius * std::sqrt(3);
        py += radius;
    }

    // 8-ball must be at the center of the triangle
    std::swap(balls[7].posData, balls[10].posData);

    // Oposite signs at the corners
    std::swap(balls[4].posData, balls[8].posData);
}



void Game::create_cue_ball()
{
    // Create and place the cue ball
    cueball = Ball{};
    cueball.setTex("./pool_assets/ball0.png");
    cueball.id = 0;
    cueball.mass = 11.0;
    cueball.addObserver(&collobserver);
    cueball.addObserver(&audio);

    auto pos = get_safe_pos();
    cueball.setPos(pos.first, pos.second);
}

void Game::check_pocket(Ball& b)
{
    static double posX =30; 
    static double posXS = 570;
    double posY = 50;
    if (tab.is_pocketed(b)) // hit the pocket
    {
        b.notify(Event::UNIT_POCKET_COLLIDED);
        b.removeObserver(&collobserver);
        b.is_movable = false;
        // b.is_visible = false;
        if (b.id!=0){
            switch(b.is_Stripes()){
                case true : 
                    b.setPos(posX,posY);
                    b.render();
                    posX += b.posData.radius * 2.5;
                    break;
                case false :
                    b.setPos(posXS,posY);
                    b.render();
                    posXS -= b.posData.radius * 2.5;
                    break;
            }
            
        }
        pockets.push_back(b.id);
    }
}

// Get a safe position to place a ball that returns to the table
std::pair<double, double> Game::get_safe_pos()
{
    double posX = 600.0;
    double posY = tab.getY() + tab.getHeight()/2 - 2.5;

    bool shifted = true;
    while (shifted)
    {
        shifted = false;

        for (auto& b: balls)
            if (b.is_visible)
                if (std::hypot(b.posData.pos_x - posX, b.posData.pos_y - posY) < b.posData.radius*2)
                {
                    posX -= b.posData.radius * 2.0;
                    shifted = true;
                    break;
                }

        if (std::hypot(cueball.posData.pos_x - posX, cueball.posData.pos_y - posY) < cueball.posData.radius*2)
        {
            posX -= cueball.posData.radius;
            shifted = true;
        }
    }

    return {posX, posY};
}

void Game::recenter_cue()
{
    cue.setPos(cueball.posData.pos_x + cueball.posData.radius, cueball.posData.pos_y - cue.getHeight()/2);
}

void Game::change_state()
{
    // Balls pocketed
    check_balls_in_pockets();
    move_was_made = false;
}

void Game::check_balls_in_pockets()
{
    // Balls pocketed
    if (pockets.size() == 0)
        return;
    else
    {
        // Cue ball in the pocket
        if (std::find(pockets.begin(), pockets.end(), 0) != pockets.end())
        {
            create_cue_ball();
            recenter_cue();
            message("Cue Ball pocketed", 2000);
        }

        bool allBallsPocketed = true;
        for (int ballId = 1; ballId <= 8; ++ballId) {
            if (std::find(pockets.begin(), pockets.end(), ballId) == pockets.end()) {
                allBallsPocketed = false;
                break;
            }
        }

        if (std::find(pockets.begin(), pockets.end(), 8) != pockets.end() and allBallsPocketed==false)
        {
            lost();
        }
        if (std::find(pockets.begin(), pockets.end(), 8) != pockets.end() and allBallsPocketed==true){
            won();
        }
        pockets.clear();
    }
}

void Game::won()
{
        message("Player Wins !!", 4000);
    GameState::next_state = GameStates::Intro;
}
void Game::lost()
{
        message("Player lost !!", 4000);
    GameState::next_state = GameStates::Intro;
}



void Game::message(const std::string& msg, unsigned delay)
{
    Font f {};
    f.loadFromFile("./pool_assets/Debrosee-ALPnL.ttf", 20);

    Texture t {};
    t.loadFromRenderedText(msg, f, SDL_Color{0xFF,0xFF,0xFF,0xFF});

    this->render();

    t.render(static_cast<int>((mainwindow->getWidth() - t.getWidth()) / 2),
             static_cast<int>((mainwindow->getHeight() - t.getHeight()) / 2));

    mainwindow->update();

    SDL_Delay(delay);
}

void Game::shoot(double speed)
{

    double angle = (cue.getAngle() * PI) / 180.0;

    cueball.movData.speed_y = -1 * std::sin(angle) * speed;
    cueball.movData.speed_x = -1 * std::cos(angle) * speed;

    cueball.notify(Event::UNIT_CUE_COLLIDED);
    move_was_made = true;
    mouse_pressed = false;
}
