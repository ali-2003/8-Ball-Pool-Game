#ifndef CUE_H
#define CUE_H

#include "texture.h"

class Cue {
    Texture t;
    Texture t_helper;
    double posX, posY;
    double degrees;
    SDL_Point center;
    SDL_Point center_helper;

public:
    Cue();

    void render();

    void setPos(double x, double y);
    void setDeg(double angle);
    void setX(double x);

    double getHeight() const noexcept {return t.getHeight();}
    double  getWidth() const noexcept {return t.getWidth();}
    double  getAngle() const noexcept {return degrees;}
    double      getX() const noexcept {return posX;}
    double      getY() const noexcept {return posY;}
};

#endif // CUE_H
