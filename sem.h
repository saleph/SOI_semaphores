#ifndef SEM_H
#define SEM_H

#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <time.h>

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
