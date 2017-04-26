#include "myqueue.h"

MyQueue::MyQueue()
    : size(0), printfMutex(1), condReadStats(1)
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
    condReadStats.p();
    ++aReads;
    condReadStats.v();

    return data;
}

Data MyQueue::readAsAorC() {
    enter();
    Data data;
    // give exclusive for {A, C}
    // no of A/C is being favorized (kernel's scheduler provides ordering)

    condACExclusion.p();
    if (condBhaveRead.getValue() == 0) {
        // B have NOT read
        condAChaveRead.v();
        mutex.v();
        condEmpty.p();
        mutex.p();

        printf("\t ACread\n");
        data = takeFirst();

        condReadyToPop.v();
    } else {
        // B have read
        condBhaveRead.p(); // consumption
        mutex.v();
        condReadyToPop.p();
        mutex.p();

        printf("\t ACpoped\n");
        data = pop();

        condACExclusion.v();
        condBExclusion.v();
        condFull.signal();
    }

    leave();
    return data;
}

Data MyQueue::readAsB() {
    Data data;
    condBExclusion.p();
    mutex.p();
    if (condAChaveRead.getValue() == 0) {
        // B is first reader
        condBhaveRead.v();
        mutex.v();
        condEmpty.p();
        mutex.p();

        data = takeFirst();
        printf("\t Bread\n");
        condReadyToPop.v();
        mutex.v();
    } else {
        // A or C have read
        condAChaveRead.p(); // consumption
        mutex.v();
        condReadyToPop.p();
        mutex.p();

        printf("\t Bpoped\n");
        data = pop();

        condBExclusion.v();
        condACExclusion.v();
        condFull.v();
        mutex.v();
        //condWaitingForEmpty.v();
    }


#if LOG
    printfMutex.p();
    printf("B: %d\n", data.val);
    printfMutex.v();
#endif
    condReadStats.p();
    ++bReads;
    condReadStats.v();

    return data;
}

Data MyQueue::readAsC() {
    Data data = readAsAorC();
#if LOG
    printfMutex.p();
    printf(" C: %d\n", data.val);
    printfMutex.v();
#endif
    condReadStats.p();
    ++cReads;
    condReadStats.v();

    return data;
}

void MyQueue::write(const Data &data) {
    condFull.p();
    mutex.p();

    push(data);
    if (size > MYQUEUE_MIN_SIZE) {
        condEmpty.v();
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
    condReadStats.p();
    printf("  A: %d, B: %d, C: %d\n", aReads, bReads, cReads);
    condReadStats.v();
    printfMutex.v();
}
