#ifndef MONITOR_H
#define MONITOR_H

#include "sem.h"
#include "conditionvariable.h"

class ConditionVariable;

class Monitor
{
    friend class ConditionVariable;
    Sem sem;
public:
    Monitor();
    virtual ~Monitor();

    void enter();
    void leave();
    void wait(ConditionVariable &cond);
    void signal(ConditionVariable &cond);
};



#endif // MONITOR_H
