// PHOTON INTERNEL "Wave" Kernel
// CODE PORTED FROM VINIX Copyright (C) 2021-2022 The Vinix authors.
// Code is governed by the GPL-2.0 license.
#include <stdint.h>
#include <stdbool.h>
#include "serial.h"
#include "utils.h"
#include "../limine.h"
#include "pmm.h"
#include "klock.h"
#include <stddef.h>
struct PageMap
{
    struct lock l;
    uint64_t *top_level;
    void *mmap_ranges[];
};
uint64_t page_size = 0x1000;
uint64_t higher_half = 0xffff800000000000;
struct PageMap kernel_pagemap;
bool vmm_initialized = false;
const uint64_t pte_present = (uint64_t)1 << 0;
const uint64_t pte_writable = (uint64_t)1 << 1;
const uint64_t pte_user = (uint64_t)1 << 2;

struct PageMap *new_pagemap()
{
    // some dumb bug where because yall adding 1 which does get 0'd out by count it's all 0's, so we just using memory address as a hack
    // this is dumb and probs breaks the system somehow but it works for now
    // hopefully some dude can fix it later on the github with a pr or something
    // please whoever sees this comment, please fix it
    uint64_t top_level = (uint64_t)pmm_alloc(1);
    if (top_level == 0)
    {
        return NULL;
    }
}