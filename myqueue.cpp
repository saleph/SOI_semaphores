#include "myqueue.h"

MyQueue::MyQueue()
    : size(0), semBhaveRead(0), semAandC(1),
      semAChaveRead(0),
      semFull(MYQUEUE_MAX_SIZE), semEmpty(0), mutex(1),
      printfMutex(1),
      aReads(0), bReads(0), cReads(0), semReadStats(1),
      semBExclusion(1), semACExclusion(1), semWaitingForEmpty(1), semReadyToPop(0)
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

    semACExclusion.p();
    mutex.p();
    if (semBhaveRead.getValue() == 0) {
        // B have NOT read
        semAChaveRead.v();
        mutex.v();
        semEmpty.p();
        mutex.p();

        printf("\t ACread\n");
        data = takeFirst();

        semReadyToPop.v();

        mutex.v();

        //semBhaveRead.p(); // wait for read in B
    } else {
        // B have read
        semBhaveRead.p(); // consumption
        mutex.v();
        semReadyToPop.p();
        mutex.p();

        printf("\t ACpoped\n");
        data = pop();

        semACExclusion.v();
        semBExclusion.v();
        semFull.v();
        mutex.v();
    }
    return data;
}

Data MyQueue::readAsB() {
    Data data;
    semBExclusion.p();
    mutex.p();
    if (semAChaveRead.getValue() == 0) {
        // B is first reader
        semBhaveRead.v();
        mutex.v();
        semEmpty.p();
        mutex.p();

        data = takeFirst();
        printf("\t Bread\n");
        semReadyToPop.v();
        mutex.v();
    } else {
        // A or C have read
        semAChaveRead.p(); // consumption
        mutex.v();
        semReadyToPop.p();
        mutex.p();

        printf("\t Bpoped\n");
        data = pop();

        semBExclusion.v();
        semACExclusion.v();
        semFull.v();
        mutex.v();
        //semWaitingForEmpty.v();
    }


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
