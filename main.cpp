#include <stdio.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <signal.h>
#include <sys/mman.h>
#include <sys/stat.h> /* For mode constants */
#include <fcntl.h> /* For O_* constants */
#include <stdlib.h>
#include <cstring>
#include "myqueue.h"
#include "sharedmemory.h"

#define CYCLES 25
#define LOG_PERIOD 2000

void performReaderA(MyQueue &queue) {
    int i = CYCLES;
    while(--i) {
        queue.readAsA();

        usleep(rand() % 300);
    }
}

void performReaderB(MyQueue &queue) {
    int i = CYCLES;
    while(--i) {
        queue.readAsB();
        usleep(rand() % 300);
    }
}

void performReaderC(MyQueue &queue) {
    int i = CYCLES;
    while(--i) {
        queue.readAsC();
        usleep(rand() % 300);
    }
}

void performWriter(MyQueue &queue) {
    Data d;
    int i = CYCLES;
    while(--i) {
        d.val = i;
        queue.write(d);
        if (i % LOG_PERIOD == 0) {
            //queue.printReadStats();
        }
        usleep(rand() % 300);
    }
}

int main ()
{
    srand(time(NULL));

    // should be constructed only in
    SharedMemory shm(sizeof(MyQueue));

    pid_t pid;
    pid_t child[4];

    /* Write a string to the shared memory segment.  */
    memcpy(shm.getPtr(), (char*)(new MyQueue), sizeof(MyQueue));

    // spawning processes
    if((pid = fork()) == 0) {
        MyQueue *q = (MyQueue*)shm.getPtr();
        performReaderA(*q);
        return 0;
    }
    child[0] = pid;


    if((pid = fork()) == 0) {
        MyQueue *q = (MyQueue*)shm.getPtr();
        performReaderB(*q);
        return 0;
    }
    child[1] = pid;


    if((pid = fork()) == 0) {
        MyQueue *q = (MyQueue*)shm.getPtr();
        performReaderC(*q);
        return 0;
    }
    child[2] = pid;


    if((pid = fork()) == 0) {
        MyQueue *q = (MyQueue*)shm.getPtr();
        performWriter(*q);
        return 0;
    }
    child[3] = pid;


    // end of program
    while (getchar() != 'q');
    ((MyQueue*)shm.getPtr())->printReadStats();

    kill(child[0], SIGKILL);
    kill(child[1], SIGKILL);
    kill(child[2], SIGKILL);
    kill(child[3], SIGKILL);


    // delete queue
    ((MyQueue*)shm.getPtr())->~MyQueue();

    return 0;
}
