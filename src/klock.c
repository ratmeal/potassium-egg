// PHOTON INTERNEL "Wave" Kernel
// CODE PORTED FROM VINIX Copyright (C) 2021-2022 The Vinix authors.
// Code is governed by the GPL-2.0 license.

#include <stdbool.h>
#include <stdint.h>
#include "serial.h"
#include "utils.h"
#include <stdatomic.h>
struct lock
{
    atomic_bool locked;
    uint64_t caller;
};

void release(struct lock *l)
{
    // Stolen Brutal Code lmao
    // Idk how long this might last but good enough for now 
    atomic_thread_fence(memory_order_seq_cst);
    l->locked = false;
}
bool test_and_acquire(struct lock *l)
{
    bool expected = false;
    uint64_t caller = (uint64_t)__builtin_return_address(0);
    
    bool ret = atomic_compare_exchange_strong(&l->locked, &expected, true);

    // test without

    //atomic_thread_fence(memory_order_seq_cst);
    if (ret == true)
    {
        l->caller = caller;
    }
    return ret;
}
void acquire(struct lock *l) 
{
    uint64_t caller = (uint64_t)__builtin_return_address(0);
    for (uint64_t i; i < (uint64_t)50000000; i++)
    {
        if (test_and_acquire(l) == true) 
        {
            l->caller = caller;
            return;
        };
        __asm__ volatile ("pause");
    };
    serial_print("lock acquire failed\n");
    serial_print("caller: ");
    char str[20];
    hex_to_str(caller, str);
    serial_print(str);
    serial_print("this is a deadlock, we gonna fault bro r u n\n");
};
