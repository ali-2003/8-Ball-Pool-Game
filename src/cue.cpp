#include "cue.h"

Cue::Cue()
    : t{},t_helper{}, posX{0.0}, posY{0.0}, degrees{0.0}, center{}
{
    t.loadFromFile("./pool_assets/cue.png");
    t_helper.loadFromFile("./pool_assets/guidanceLine.png");

    center = {-10, static_cast<int>(t.getHeight()/2)};
    center_helper = {-10, static_cast<int>(t_helper.getHeight()/2)};
}

void Cue::render()
{
    int distance = 100;
    t.render(static_cast<int>(posX), static_cast<int>(posY), nullptr, degrees, &center, SDL_FLIP_HORIZONTAL);
    int guideX = static_cast<int>(posX) + cos(degrees * M_PI / 180) * distance;
    int guideY = static_cast<int>(posY) + sin(degrees * M_PI / 180) * distance;

    // Render the guidance line
    // t_helper.render(guideX, guideY, nullptr, degrees, &center_helper, SDL_FLIP_HORIZONTAL);
}

void Cue::setPos(double x, double y)
{
    posX = x;
    posY = y;
}

void Cue::setDeg(double angle)
{
    degrees = angle;
}

void Cue::setX(double x)
{
    posX = x;
}
