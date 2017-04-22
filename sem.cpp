#include "sem.h"


Sem::Sem(int val) {
    sem_init(&s, 1, val);
}

Sem::~Sem() {
    sem_destroy(&s);
}

void Sem::p() {
    sem_wait(&s);
}

void Sem::v() {
    sem_post(&s);
}

int Sem::getVal() {
    int val;
    sem_getvalue(&s, &val);
    return val;
}
