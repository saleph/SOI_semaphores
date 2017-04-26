#include "conditionvariable.h"

ConditionVariable::ConditionVariable()
    : sem(0), numberOfWaiting(0)
{ }

void ConditionVariable::wait() {
    sem.p();
}

bool ConditionVariable::signal() {
    if (numberOfWaiting > 0) {
        --numberOfWaiting;
        sem.v();
        return true;
    }
    return false;
}
