#include "collisionobserver.h"
#include "rect.h"
#include "ball.h"
#include "Unit.h"

#include <cmath>

CollisionObserver::CollisionObserver() {}

void CollisionObserver::onNotify(Unit& entity, Event event)
{
    if (event == Event::UNIT_MOVED)
        checkCollisions(dynamic_cast<Ball*>(&entity));
}

double CollisionObserver::distanceSquared(double x1, double y1, double x2, double y2)
{
    double deltaX = x2 - x1;
    double deltaY = y2 - y1;

    return deltaX*deltaX + deltaY*deltaY;
}

// Check if 2 balls collided
bool CollisionObserver::collided(Ball* b, Ball* b2)
{
    double totalRadiusSquared = b->posData.radius + b2->posData.radius;
    totalRadiusSquared = totalRadiusSquared * totalRadiusSquared;

    return distanceSquared(b->posData.pos_x, b->posData.pos_y, b2->posData.pos_x, b2->posData.pos_y) < totalRadiusSquared;
}

// Check is Ball* b collided with Rect* r
bool CollisionObserver::collided(Ball* b, Rect* r)
{
    //Closest point on collision box
    auto closer = find_closer(b, r);

    return distanceSquared(closer.first, closer.second, b->posData.pos_x, b->posData.pos_y) < b->posData.radius * b->posData.radius;
}

// Check collision of this Ball* b with every UNIT watched
void CollisionObserver::checkCollisions(Ball* b)
{
    if (b == nullptr) return;

    for (Unit* sub: observed_)
    {
        Ball* b2 = dynamic_cast<Ball*>(sub);
        if (b2 != nullptr)
        {
            if (b2 != b)
            {
                if (collided(b, b2))
                {
                    fix(b, b2);

                    if (b2->id == 999) // Side of the pocket
                        b->notify(Event::UNIT_WALL_COLLIDED);
                    else // Other ball
                        b->notify(Event::UNIT_BALL_COLLIDED);
                }
            }
        }
        else
        {
            Rect* r = dynamic_cast<Rect*>(sub);
            if (r != nullptr)
            {
                if (collided(b, r))
                {
                    // std::cout << "Collided with wall!!" << std::endl;
                    fix(b, r);
                    b->notify(Event::UNIT_WALL_COLLIDED);
                }
            }
        }
    }
}

// Find closest point on collision box
std::pair<int, int> CollisionObserver::find_closer(Ball* b, Rect* rec)
{
    int cX, cY;

    //Find closest x offset
    if(b->posData.pos_x < rec->rect.x)
        cX = rec->rect.x;
    else if(b->posData.pos_x > rec->rect.x + rec->rect.w )
        cX = rec->rect.x + rec->rect.w;
    else
        cX = static_cast<int>(b->posData.pos_x);

    //Find closest y offset
    if(b->posData.pos_y < rec->rect.y)
        cY = rec->rect.y;
    else if( b->posData.pos_y > rec->rect.y + rec->rect.h )
        cY = rec->rect.y + rec->rect.h;
    else
        cY = static_cast<int>(b->posData.pos_y);

    return {cX, cY};
}

// Fix movement of the ball when collided with wall
void CollisionObserver::fix(Ball* b, Rect* rec)
{
    auto closer = find_closer(b, rec);
    int cX = closer.first, cY = closer.second;

    if (b->movData.speed_y > 0)
    {
        if (cY == rec->rect.y) // collided from above
        {
            b->movData.speed_y *= -1;
            b->movData.speed_y *= 0.8;
            b->posData.pos_y = cY - b->posData.radius;
        }
        else // collided from side
        {
            b->movData.speed_x *= -1;
            b->movData.speed_x *= 0.8;
            if (b->movData.speed_x > 0) // from left side
                b->posData.pos_x = cX + b->posData.radius;
            else // from right side
                b->posData.pos_x = cX - b->posData.radius;
        }
    }
    else
    {
        if (cY == rec->rect.y + rec->rect.h) //collided from bellow
        {
            b->movData.speed_y *= -1;
            b->movData.speed_y *= 0.8;
            b->posData.pos_y = cY + b->posData.radius;
        }
        else // collided from side
        {
            b->movData.speed_x *= -1;
            b->movData.speed_x *= 0.8;

            if (b->movData.speed_x > 0) // from left side
                b->posData.pos_x = cX + b->posData.radius;
            else // from right side
                b->posData.pos_x = cX - b->posData.radius;
        }
    }
}


void collision2Ds(double m1, double m2, double R,
                  double x1, double y1, double x2, double y2,
                  double& vx1, double& vy1, double& vx2, double& vy2)
{
    double  m21,dvx2,a,x21,y21,vx21,vy21,fy21,sign,vx_cm,vy_cm;

    m21=m2/m1;
    x21=x2-x1;
    y21=y2-y1;
    vx21=vx2-vx1;
    vy21=vy2-vy1;

    vx_cm = (m1*vx1+m2*vx2)/(m1+m2) ;
    vy_cm = (m1*vy1+m2*vy2)/(m1+m2) ;

    if ( (vx21*x21 + vy21*y21) >= 0) return;

    fy21=1.0E-12*std::fabs(y21);
    if ( std::fabs(x21)<fy21 ) {
        if (x21<0) { sign=-1; }
        else { sign=1;}
        x21=fy21*sign;
    }

    //     ***  update velocities ***
    a=y21/x21;
    dvx2= -2*(vx21 +a*vy21)/((1+a*a)*(1+m21)) ;
    vx2=vx2+dvx2;
    vy2=vy2+a*dvx2;
    vx1=vx1-m21*dvx2;
    vy1=vy1-a*m21*dvx2;

    //     ***  velocity correction for inelastic collisions ***
    vx1=(vx1-vx_cm)*R + vx_cm;
    vy1=(vy1-vy_cm)*R + vy_cm;
    vx2=(vx2-vx_cm)*R + vx_cm;
    vy2=(vy2-vy_cm)*R + vy_cm;

    return;
}


void CollisionObserver::fix(Ball* b, Ball* b2)
{
    collision2Ds(b->mass, b2->mass, 0.92, b->posData.pos_x, b->posData.pos_y,
                 b2->posData.pos_x, b2->posData.pos_y, b->movData.speed_x, b->movData.speed_y,
                 b2->movData.speed_x, b2->movData.speed_y);
}


