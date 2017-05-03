#ifndef MONITOR_H
#define MONITOR_H

#include "sem.h"
#include "conditionvariable.h"

class ConditionVariable;
class Guard;

class Monitor
{
    friend class ConditionVariable;
    friend class Guard;
    Sem sem;

protected:
    Monitor();
    virtual ~Monitor();
    void wait(ConditionVariable &cond);
    void signal(ConditionVariable &cond);

    class Guard {
        Monitor *monitor;
    public:
        Guard(Monitor *m) : monitor(m) {
            monitor->sem.p();
        }
        ~Guard() {
            monitor->sem.v();
        }
    };
};



#endif // MONITOR_H
