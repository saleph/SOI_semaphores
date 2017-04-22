#include "myqueue.h"

MyQueue::MyQueue()
    : size(0), semCompleteRead(0), semAandC(1),
      semB(0),
      semFull(MYQUEUE_MAX_SIZE), semEmpty(0), mutex(1), printfMutex(1) {
}

MyQueue::~MyQueue() {
}

Data MyQueue::readAsA() {
    Data data;
    semAandC.p(); // give exclusive from {A, C}
    semEmpty.p(); // anything to read
    mutex.p();

    data = takeFirst();
    lastReader = READER_A;

    semB.v();     // wakeup B
    mutex.v();

    semCompleteRead.p(); // wait for read in B
    printfMutex.p();
    printf("A: %d\n", data.val);
    printfMutex.v();
    semAandC.v();

    return data;
}

Data MyQueue::readAsB() {
    printSems();

    Data data;
    semB.p();   // semB is slave - it always wait for A/C, which perform read first
                // no semEmpty required - it was checked by A/C
    mutex.p();

    data = pop();

    printfMutex.p();
    printf(" B: %d\n", data.val);
    printfMutex.v();

    semFull.v();
    semCompleteRead.v(); // wakeup A/C waiting for B
    mutex.v();

    return data;
}

Data MyQueue::readAsC() {
    Data data;

    semAandC.p(); // give exclusive from {A, C}
    semEmpty.p(); // anything to read
    mutex.p();

    data = takeFirst();
    lastReader = READER_C;

    semB.v();     // wakeup B
    mutex.v();

    semCompleteRead.p(); // wait for read in B
    printfMutex.p();
    printf("C: %d\n", data.val);
    printfMutex.v();
    semAandC.v();

    return data;
}

void MyQueue::write(const Data &data) {
    semFull.p();
    mutex.p();

    push(data);
    if (size > MYQUEUE_MIN_SIZE) {
        semEmpty.v();
    }
    printfMutex.p();
    printf("  Writer: %d\n", data.val);
    printfMutex.v();

    mutex.v();
}

Data MyQueue::takeFirst() {
    return queue[0];
}

Data MyQueue::pop() {
    Data first = queue[0];
    --size;
    for (int i=0; i < size; ++i) {
        queue[i] = queue[i+1];
    }
    return first;
}

void MyQueue::push(const Data &data) {
    queue[size++] = data;
}

void MyQueue::printSems() {
//    printfMutex.p();
//    printf("\tsemA: %d\n", semA.getVal());
//    printf("\tsemB: %d\n", semB.getVal());
//    printf("\tsemC: %d\n", semC.getVal());
//    printfMutex.v();
}
