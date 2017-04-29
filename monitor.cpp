#include "monitor.h"

Monitor::Monitor()
    : sem(1)
{ }

Monitor::~Monitor()
{ }

void Monitor::enter() {
    sem.p();
}

void Monitor::leave() {
    sem.v();
}

void Monitor::wait(ConditionVariable &cond) {
    ++cond.numberOfWaiting;
    leave();
    cond.wait();
}

void Monitor::signal(ConditionVariable &cond) {
    if (cond.signal()) {
        enter(); // give the awoken thread access to crit section
    }
}
