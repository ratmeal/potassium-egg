// PHOTON INTERNEL "Wave" Kernel
// CODE PORTED FROM VINIX Copyright (C) 2021-2022 The Vinix authors.
// Code is governed by the GPL-2.0 license.
#include <stdint.h>
#include <stdbool.h>
#include "serial.h"
#include "utils.h"
#include "klock.h"
#include "../limine.h"
struct PageMap
{
    struct lock l;
    uint64_t *top_level;
    void *mmap_ranges[];
};
uint64_t page_size = 0x1000;
uint64_t higher_half = 0xffff800000000000;
struct PageMap kernel_pagemap;