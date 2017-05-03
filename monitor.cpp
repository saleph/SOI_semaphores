#include "monitor.h"

Monitor::Monitor()
    : sem(1)
{ }

Monitor::~Monitor()
{ }

void Monitor::wait(ConditionVariable &cond) {
    ++cond.numberOfWaiting;
    sem.v();
    cond.wait();
}

void Monitor::signal(ConditionVariable &cond) {
    if (cond.signal()) {
        sem.p(); // give the awoken thread access to crit section
    }
}
