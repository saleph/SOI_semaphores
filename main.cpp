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

MyQueue queue;

void *performReaderA(void*) {
    int i = CYCLES;
    while(--i) {
        queue.readAsA();
        usleep(rand() % 300);
    }
}

void *performReaderB(void*) {
    int i = CYCLES;
    while(--i) {
        queue.readAsB();
        usleep(rand() % 300);
    }
}

void *performReaderC(void*) {
    int i = CYCLES;
    while(--i) {
        queue.readAsC();
        usleep(rand() % 300);
    }
}

void *performWriter(void*) {
    Data d;
    int i = CYCLES;
    while(--i) {
        d.val = i;
        queue.write(d);
        usleep(rand() % 300);
    }
}

int main ()
{
    pthread_t readerA, readerB, readerC, writer;
    srand(time(NULL));

    pthread_create(&readerA, NULL, performReaderA, NULL);
    pthread_create(&readerB, NULL, performReaderB, NULL);
    pthread_create(&readerC, NULL, performReaderC, NULL);
    pthread_create(&writer, NULL, performWriter, NULL);

    pthread_join(readerA, NULL);
    pthread_join(readerB, NULL);
    pthread_join(readerC, NULL);
    pthread_join(writer, NULL);

    return 0;
}
