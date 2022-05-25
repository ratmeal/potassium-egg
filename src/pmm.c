// PHOTON INTERNEL "Wave" Kernel
// CODE PORTED FROM VINIX Copyright (C) 2021-2022 The Vinix authors.
// Code is governed by the GPL-2.0 license.


#include <stdint.h>
#include <stdbool.h>
#include "serial.h"
#include "utils.h"
#include "klock.h"
#include "../limine.h"
#include "vmm.h"
// shut up it didn't work in the .h file so I'm putting it here

struct lock pmm_lock;
void *pmm_bitmap;
uint64_t pmm_avl_page_count;
uint64_t pmm_last_used_index;
uint64_t free_pages;
struct limine_memmap_request memmap_req = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};
void print_free()
{
    acquire(&pmm_lock);
    serial_print("Free Pages: ");
    char str[20];
    hex_to_str(free_pages, str);
    serial_print(str);
    serial_print("\n");
    release(&pmm_lock);
};
void pmm_init()
{
    
    uint64_t highest_address = 0;
    for (int i = 0; i < memmap_req.response->entry_count; i++)
    {
        serial_print("memmap entry lol\n");
        if (memmap_req.response->entries[i]->type != (uint32_t)LIMINE_MEMMAP_USABLE && memmap_req.response->entries[i]->type != (uint32_t)LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE)
        {
            continue;
        };
        if (memmap_req.response->entries[i]->base + memmap_req.response->entries[i]->length > highest_address)
        {
            highest_address = memmap_req.response->entries[i]->base + memmap_req.response->entries[i]->length;
        };
    }
    pmm_avl_page_count = highest_address / page_size;
    uint64_t bitmap_size = align_up(pmm_avl_page_count / 8, page_size);
    serial_print("bitmap calculated!\n");
    for (int i = 0; i < memmap_req.response->entry_count; i++)
    {
        if (memmap_req.response->entries[i]->type != (uint32_t)LIMINE_MEMMAP_USABLE)
        {
            continue;
        };
        if (memmap_req.response->entries[i]->length >= bitmap_size)
        {
            pmm_bitmap = (void*)memmap_req.response->entries[i]->base + higher_half;
            memset(pmm_bitmap, 0xff, bitmap_size);
            memmap_req.response->entries[i]->length -= bitmap_size;
            memmap_req.response->entries[i]->base += bitmap_size;
            break;
        }
    };
    for (int i = 0; i < memmap_req.response->entry_count; i++)
    {
        if (memmap_req.response->entries[i]->type != (uint32_t)LIMINE_MEMMAP_USABLE)
        {
            continue;
        };
        for (uint64_t j = 0; j < memmap_req.response->entries[i]->length; j += page_size)
        {
            free_pages++;
            bitreset(pmm_bitmap, (memmap_req.response->entries[i]->base + j) / page_size);
        }
    }
    print_free();
};