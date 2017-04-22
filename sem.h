#ifndef SEM_H
#define SEM_H

#include <semaphore.h>

class Sem
{
    sem_t s;
public:
    Sem(int val);
    ~Sem();

    void p();
    void v();
    int getVal();
};

#endif // SEM_H
