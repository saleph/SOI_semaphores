#include "myqueue.h"

MyQueue::MyQueue()
    : size(0), printfMutex(1), condReadStats(1), lastReader(NoReader)
{
}

MyQueue::~MyQueue() {
    printReadStats();
}

Data MyQueue::readAsA() {
    Data data = readAsAorC();
    enter();
#if LOG
    printf(" A: %d\n", data.val);
#endif
    ++aReads;
    leave();

    return data;
}

Data MyQueue::readAsAorC() {
    enter();
    Data data;

    while (acIsConsuming) {
        wait(condACExclusion);
    }
    acIsConsuming = true;

    if (bHaveRead) {
        bHaveRead = false; // consumption via B
        while (!readyToPop) {
            wait(condReadyToPop);
        }
        readyToPop = false;

#if LOG

#endif
#if LOG
        printf("==== AC poped\n");
#endif
        data = pop();

        acIsConsuming = false;
        bIsConsuming = false;
        signal(condACExclusion);
        signal(condBExclusion);
        signal(condFull);
    } else {
        acHaveRead = true;
        signal(condAChaveRead);
        while (size <= MYQUEUE_MIN_SIZE) {
            wait(condEmpty);
        }
#if LOG
        printf("\t AC read\n");
#endif

        data = takeFirst();

        readyToPop = true;
        signal(condReadyToPop);
    }

    leave();
    return data;
}

Data MyQueue::readAsB() {
    enter();
    Data data;

    while (bIsConsuming) {
        wait(condBExclusion);
    }
    bIsConsuming = true;

    if (acHaveRead) {
        acHaveRead = false; // consumtion
        while (!readyToPop) {
            wait(condReadyToPop);
        }
        readyToPop = false;
#if LOG
        printf("=== B poped\n");
#endif

        data = pop();

        bIsConsuming = false;
        acIsConsuming = false;
        signal(condBExclusion);
        signal(condACExclusion);
        signal(condFull);
    } else {
        // B is first reader
        bHaveRead = true;
        signal(condBhaveRead);
        while (size <= MYQUEUE_MIN_SIZE) {
            wait(condEmpty);
        }

        data = takeFirst();
#if LOG
        printf("\t B read\n");
#endif

        readyToPop = true;
        signal(condReadyToPop);
    }


#if LOG
    printf("B: %d\n", data.val);
#endif
    ++bReads;

    leave();
    return data;
}

Data MyQueue::readAsC() {
    Data data = readAsAorC();
    enter();
#if LOG
    printf(" C: %d\n", data.val);
#endif
    ++cReads;
    leave();

    return data;
}

void MyQueue::write(const Data &data) {
    enter();

    while (size == MYQUEUE_MAX_SIZE) {
        wait(condFull);
    }

    push(data);
    if (size > MYQUEUE_MIN_SIZE) {
        signal(condEmpty);
    }

#if LOG
    printf("  Writer: %d\n", data.val);
#endif
    leave();
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
    enter();
    printf("  A: %d, B: %d, C: %d\n", aReads, bReads, cReads);
    leave();
}
