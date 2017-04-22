#include "myqueue.h"

MyQueue::MyQueue()
    : size(0), semCompleteRead(0), semAandC(1), semB(0), semFull(QUEUE_MAX_SIZE), semEmpty(0), mutex(1) {
}

MyQueue::~MyQueue() {
}

Data MyQueue::readAsA() {
    return readAsAorC();
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

    return data;
}

Data MyQueue::readAsC() {
    return readAsAorC();
}

void MyQueue::write(const Data &data) {
    semFull.p();
    mutex.p();

    push(data);
    if (size > QUEUE_MIN_SIZE) {
        semEmpty.v();
    }

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
