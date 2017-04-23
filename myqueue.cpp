#include "myqueue.h"

MyQueue::MyQueue()
    : size(0), semCompleteRead(0), semAandC(1),
      semB(0),
      semFull(MYQUEUE_MAX_SIZE), semEmpty(0), mutex(1),
      printfMutex(1),
      aReads(0), bReads(0), cReads(0), semReadStats(1)
{
}

MyQueue::~MyQueue() {
}

Data MyQueue::readAsA() {
    Data data = readAsAorC();
#if LOG
    printfMutex.p();
    printf(" A: %d\n", data.val);
    printfMutex.v();
#endif
    semReadStats.p();
    ++aReads;
    semReadStats.v();

    return data;
}

Data MyQueue::readAsAorC() {
    Data data;
    // give exclusive for {A, C}
    // no of A/C is being favorized (kernel's scheduler provides ordering)
    semAandC.p();
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
                // no semEmpty required - it was already checked by A/C
    mutex.p();

    data = pop();
    semFull.v();
    semCompleteRead.v(); // wakeup A/C waiting for B
    mutex.v();
#if LOG
    printfMutex.p();
    printf("B: %d\n", data.val);
    printfMutex.v();
#endif
    semReadStats.p();
    ++bReads;
    semReadStats.v();

    return data;
}

Data MyQueue::readAsC() {
    Data data = readAsAorC();
#if LOG
    printfMutex.p();
    printf(" C: %d\n", data.val);
    printfMutex.v();
#endif
    semReadStats.p();
    ++cReads;
    semReadStats.v();

    return data;
}

void MyQueue::write(const Data &data) {
    semFull.p();
    mutex.p();

    push(data);
    if (size > MYQUEUE_MIN_SIZE) {
        semEmpty.v();
    }

    mutex.v();
#if LOG
    printfMutex.p();
    printf("  Writer: %d\n", data.val);
    printfMutex.v();
#endif
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

void MyQueue::printReadStats() {
    printfMutex.p();
    semReadStats.p();
    printf("  A: %d, B: %d, C: %d\n", aReads, bReads, cReads);
    semReadStats.v();
    printfMutex.v();
}
