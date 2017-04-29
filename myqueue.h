#ifndef MYQUEUE_H
#define MYQUEUE_H

#include <stdio.h>
#include <unistd.h>
#include "monitor.h"
#include "conditionvariable.h"

#define MYQUEUE_MAX_SIZE 4
#define MYQUEUE_MIN_SIZE 3

#define LOG 0


typedef struct Data {
    int val;
} Data;

class MyQueue : Monitor
{
    struct Data queue[MYQUEUE_MAX_SIZE];
    int size;
    ConditionVariable condBhaveRead, condAChaveRead, condFull, condEmpty, mutex, condBExclusion, condACExclusion, condWaitingForEmpty, condReadyToPop;
    bool bHaveRead, acHaveRead, bIsConsuming, acIsConsuming, readyToPop;

    Sem printfMutex, condReadStats;
    int aReads, bReads, cReads;


    class Guard {
        ConditionVariable &condition;
    public:
        Guard(ConditionVariable &c)
            : condition(c) {
        }
    };
public:
    MyQueue();
    virtual ~MyQueue();

    Data readAsA();
    Data readAsB();
    Data readAsC();
    void write(const Data &data);

    void printReadStats();
private:
    Data takeFirst();
    Data pop();
    void push(const Data &data);
    Data readAsAorC();
};


#endif // MYQUEUE_H
