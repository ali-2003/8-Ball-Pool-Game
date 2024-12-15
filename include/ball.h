#ifndef BALL_H
#define BALL_H

#include "Unit.h"
#include "texture.h"

struct Circle {
    double pos_x;
    double pos_y;
    double radius;
};

class Ball : public Unit  {
    Texture texture;
public:
    bool is_movable = true;
    bool is_visible = true;
    Circle posData;
    MovementData movData;
    double mass = 10.0;
    int id = 0; // the number of the ball
    Ball();
    Ball(const std::string& path);
    void setTex(const std::string& path);
    void setPos(double px, double py);
    void setVel(double sx, double sy);
    virtual void   move() override;
    virtual void render() override;
    bool is_moving() const noexcept;
    bool is_Stripes(){return (this->id>0 &&this->id<8);}
};

#endif // BALL_H
