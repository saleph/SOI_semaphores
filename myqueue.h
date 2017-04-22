#ifndef MYQUEUE_H
#define MYQUEUE_H

#include "sem.h"

#define QUEUE_MAX_SIZE 13
#define QUEUE_MIN_SIZE 3
#define QUEUE_SIZE     0

#define NO_OF_SEM   6
#define SEM_A       0
#define SEM_B       1
#define SEM_C       2
#define SEM_FULL    3
#define SEM_EMPTY   4
#define MUTEX       5

typedef struct Data {
    int val;
} Data;

class MyQueue
{
    struct Data queue[QUEUE_MAX_SIZE];
    int size;
    Sem semCompleteRead, semAandC, semB, semFull, semEmpty, mutex;
public:
    MyQueue();
    ~MyQueue();

    Data readAsA();
    Data readAsB();
    Data readAsC();
    void write(const Data &data);

private:
    Data readAsAorC();
    Data takeFirst();
    Data pop();
    void push(const Data &data);
};

#endif // MYQUEUE_H
