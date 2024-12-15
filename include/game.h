#include "gamestate.h"
#include "texture.h"
#include "font.h"
#include "table.h"
#include "collisionobserver.h"
#include "audioobserver.h"
#include "ball.h"
#include "cue.h"
#include "score.h"

#include <vector>
#include <utility>

extern const double PI;

class Game : public GameState {
    Texture background;
    Texture scoreBoard;
    Texture score;
    Font f3;
    CollisionObserver collobserver;
    AudioObserver audio;
    Table tab;
    Score s {};
    Ball balls[15] {};
    Ball cueball {};
    Cue cue {};

    std::vector<int> pockets {};
    bool moving_state = false;
    bool player1turn = true;
    bool move_was_made = false;
    int team_color = 0; // 0 = undefined, 1 = player1 kills solid, 2 = player1 kills stripes
    bool mouse_pressed = false;

public:
    Game();

    virtual void handle_events() override;
    virtual void logic() override;
    virtual void render() override;

private:
    void handle_when_still(SDL_Event& e);
    void create_balls();
    void create_cue_ball();
    void check_pocket(Ball& b);
    std::pair<double,double> get_safe_pos();
    void recenter_cue();
    void change_state();
    void check_balls_in_pockets();
    void won();
    void lost();
    void message(const std::string& msg, unsigned delay);
    void shoot(double speed);
};


