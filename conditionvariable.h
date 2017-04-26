#ifndef CONDITIONVARIABLE_H
#define CONDITIONVARIABLE_H

#include "sem.h"

class Monitor;

class ConditionVariable
{
    friend class Monitor;
    Sem sem;
    int numberOfWaiting;

public:
    ConditionVariable();

    void wait();
    bool signal();
};

#endif // CONDITIONVARIABLE_H
