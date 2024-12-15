#include "Unit.h"

#include <algorithm>

Unit::Unit() : observers_{}
{
}

Unit::~Unit() noexcept
{
    for (Observer* ob: observers_)
        ob->unregisterSub(this);
}

void Unit::addObserver(Observer* observer)
{
    if (observer != nullptr)
    {
        bool found = std::find(std::begin(observers_), std::end(observers_), observer) != std::end(observers_);

        if (!found)
        {
            observers_.push_back(observer);
            observer->registerSub(this);
        }
    }
}

void Unit::removeObserver(Observer* observer)
{
    auto it = std::find(std::begin(observers_), std::end(observers_), observer);

    if (it != std::end(observers_))
    {
        observers_.erase(it);
        observer->unregisterSub(this);
    }
}

void Unit::notify(Event event)
{
    for (Observer* ob: observers_)
        ob->onNotify(*this, event);
}
