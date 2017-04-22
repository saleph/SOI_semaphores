#ifndef MYQUEUE_H
#define MYQUEUE_H

#include <stdio.h>
#include "sem.h"

#define MYQUEUE_MAX_SIZE 13
#define MYQUEUE_MIN_SIZE 3
#define MYQUEUE_SIZE     0


typedef struct Data {
    int val;
} Data;

class MyQueue
{
    struct Data queue[MYQUEUE_MAX_SIZE];
    int size;
    Sem semCompleteRead, semAandC, semB, semFull, semEmpty, mutex, printfMutex;
public:
    MyQueue();
    ~MyQueue();

    Data readAsA();
    Data readAsB();
    Data readAsC();
    void write(const Data &data);

private:
    Data takeFirst();
    Data pop();
    void push(const Data &data);
};

#endif // MYQUEUE_H
