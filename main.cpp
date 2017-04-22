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

#define CYCLES 25
#define SHM_NAME "/myshm"

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
        usleep(rand() % 300);
    }
}

void *getSHM() {
    int shm;
    void *memory;
    shm = shm_open(SHM_NAME, O_CREAT | O_RDWR, S_IRWXU | S_IRWXO);
    ftruncate(shm, sizeof(MyQueue));
    memory = mmap(NULL, sizeof(MyQueue), PROT_WRITE | PROT_READ, MAP_SHARED, shm, 0);
    return memory;
}

int main ()
{
    srand(time(NULL));

    pid_t pid;
    pid_t child[4];
    void* shared_memory;

    /* Write a string to the shared memory segment.  */
    shared_memory = getSHM();
    memcpy(shared_memory, (char*)(new MyQueue), sizeof(MyQueue));

    // spawning processes
    if((pid = fork()) == 0) {
        MyQueue *q = (MyQueue*)getSHM();
        performReaderA(*q);
        shmdt (shared_memory);
        return 0;
    }
    child[0] = pid;


    if((pid = fork()) == 0) {
        MyQueue *q = (MyQueue*)getSHM();
        performReaderB(*q);
        shmdt (shared_memory);
        return 0;
    }
    child[1] = pid;


    if((pid = fork()) == 0) {
        MyQueue *q = (MyQueue*)getSHM();
        performReaderC(*q);
        shmdt (shared_memory);
        return 0;
    }
    child[2] = pid;


    if((pid = fork()) == 0) {
        MyQueue *q = (MyQueue*)getSHM();
        performWriter(*q);
        shmdt (shared_memory);
        return 0;
    }
    child[3] = pid;


    // end of program
    while (getchar() != 'q');

    kill(child[0], SIGKILL);
    kill(child[1], SIGKILL);
    kill(child[2], SIGKILL);
    kill(child[3], SIGKILL);


    // delete queue
    ((MyQueue*)shared_memory)->~MyQueue();

    shm_unlink(SHM_NAME);
    return 0;
}
