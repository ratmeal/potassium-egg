// PHOTON INTERNEL "Wave" Kernel
// Code is governed by the GPL-2.0 license.

// TARGET FUNCTIONS TO IMPLMENT:
//void free(void *ptr);
//void *malloc(uint64_t size);
//void *realloc(void *ptr, uint64_t new_size);
//void *calloc(uint64_t a, uint64_t b);

// SLABS SHOULD HAVE LOCK
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "../klock/klock.h"
#include "../serial/serial.h"
#include "../../limine.h"
#include "../utils.h"
#include "vmm.h"

void *malloc(uint64_t size)
{

};
void *realloc(void *ptr, uint64_t new_size)
{
    
};
void *calloc(uint64_t a, uint64_t b)
{

};
void free(void *ptr)
{

};