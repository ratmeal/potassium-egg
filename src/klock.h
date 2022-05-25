#include <stdatomic.h>
void acquire(struct lock *l);
bool test_and_acquire(struct lock *l);
void release(struct lock *l);
struct lock
{
    atomic_bool locked;
    uint64_t caller;
};