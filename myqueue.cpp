#include "myqueue.h"

MyQueue::MyQueue()
    : size(0), semCompleteRead(0), semAandC(1), semB(0),
      semFull(MYQUEUE_MAX_SIZE), semEmpty(0), mutex(1), printfMutex(1) {
}

MyQueue::~MyQueue() {
}

Data MyQueue::readAsA() {
    Data d = readAsAorC();
    printfMutex.p();
    printf("Reader A read: %d", d.val);
    printfMutex.v();
    return d;
}

Data MyQueue::readAsAorC() {
    Data data;
    semAandC.p(); // give exclusive from {A, C}
    semEmpty.p(); // anything to read
    mutex.p();

    data = takeFirst();

    semB.v();     // wakeup B
    mutex.v();

    semCompleteRead.p(); // wait for read in B
    semAandC.v();

    return data;
}

Data MyQueue::readAsB() {
    Data data;
    semB.p();   // semB is slave - it always wait for A/C, which perform read first
                // no semEmpty required - it was checked by A/C
    mutex.p();

    data = pop();

    semFull.v();
    semCompleteRead.v(); // wakeup A/C waiting for B
    mutex.v();

    printfMutex.p();
    printf("Reader B read: %d\n", data.val);
    printfMutex.v();

    return data;
}

Data MyQueue::readAsC() {
    Data d = readAsAorC();
    printfMutex.p();
    printf("Reader C read: %d\n", d.val);
    printfMutex.v();
    return d;
}

void MyQueue::write(const Data &data) {
    semFull.p();
    mutex.p();

    push(data);
    if (size > MYQUEUE_MIN_SIZE) {
        semEmpty.v();
    }

    mutex.v();


    printfMutex.p();
    printf("Writer: %d\n", data.val);
    printfMutex.v();
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
