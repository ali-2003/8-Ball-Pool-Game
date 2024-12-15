#ifndef OBSERVER_H
#define OBSERVER_H

#include <vector>

class Unit; // Forward declare to break circular includes

enum class Event {
    UNIT_MOVED,
    UNIT_WALL_COLLIDED,
    UNIT_BALL_COLLIDED,
    UNIT_POCKET_COLLIDED,
    UNIT_CUE_COLLIDED
};

class Observer {
protected:
    std::vector<Unit*> observed_;
public:
    Observer();
    virtual ~Observer() noexcept;
    virtual void onNotify(Unit& entity, Event event) = 0;
    void registerSub(Unit* s);
    void unregisterSub(Unit* s);
};

#endif // OBSERVER_H
