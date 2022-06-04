#ifndef KLOCK.h
#define KLOCK.h
#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>
struct lock
{
    atomic_bool locked;
    uint64_t caller;
};

void acquire(struct lock l);
bool test_and_acquire(struct lock l);
void release(struct lock l);
#endif