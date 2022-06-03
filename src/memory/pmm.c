// PHOTON INTERNEL "Wave" Kernel
// Slab Allocator is ported from VINIX (Copyright (C) 2021-2022, The VINIX Project)
// Code is governed by the GPL-2.0 license.
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "../klock/klock.h"
#include "../serial/serial.h"
#include "../../limine.h"
#include "../utils.h"

extern struct limine_memmap_request memmap_request;
extern struct limine_hhdm_request hhdm_request;
struct lock *pmm_lock;
uint8_t *bitmap;
static bool pmm_init_ = false;
void pmm_init()
{
    
    acquire(&pmm_lock);
    if (pmm_init_ == true)
    {
        serial_print("oh no!\n");
        release(&pmm_lock);
        return;
    }
    // print out each entry in the memmap
    uint64_t highest_page_usable = 0;
    uint64_t last_page_last_entry = 0;
    for (uint64_t i = 0; i < memmap_request.response->entry_count; i++)
    {
        uint64_t last_page = memmap_request.response->entries[i]->base + memmap_request.response->entries[i]->length - 4096;
        if (last_page > highest_page_usable)
        {
            if (memmap_request.response->entries[i]->type == LIMINE_MEMMAP_USABLE)
            {
                highest_page_usable = last_page;
            }
        }
        if (last_page_last_entry != last_page)
        {
            last_page_last_entry = last_page;
        }
        // calculate the last entries last page
    }
    uint64_t bitmap_size = (highest_page_usable / 4096) / 8;
    uint64_t bitmap_size_aligned = align_up(bitmap_size, 4096);
    uint64_t bitmap_base;
    for (int i = 0; i < memmap_request.response->entry_count; i++)
    {
        if (memmap_request.response->entries[i]->type == LIMINE_MEMMAP_USABLE)
        {
            if (memmap_request.response->entries[i]->length >= bitmap_size_aligned)
            {
               bitmap_base = memmap_request.response->entries[i]->base + hhdm_request.response->offset;
               memmap_request.response->entries[i]->base += bitmap_size_aligned;
               memmap_request.response->entries[i]->length -= bitmap_size_aligned;
               break;
            }
        }
    }
    serial_print("BitMap Allocated!\n");
    // make a pointer using the base of the bitmap
    bitmap = (uint8_t*)(bitmap_base);
    // set the bitmap to 0
    memset(bitmap, 0, bitmap_size_aligned);
    for (int i = 0; i < memmap_request.response->entry_count; i++)
    {
        if (memmap_request.response->entries[i]->type == LIMINE_MEMMAP_USABLE)
        {
            // loop through each page in the entry
            for (uint64_t j = 0; j < memmap_request.response->entries[i]->length; j += 4096)
            {
                // get the index for the bitmap
                uint64_t index = j / 4096;
                
                bitmap[index] = 0;
            }
        }
        else
        {
            // loop through each page in the entry
            for (uint64_t j = 0; j < memmap_request.response->entries[i]->length; j += 4096)
            {
                // get the index for the bitmap
                uint64_t index = j / 4096;
                
                bitmap[index] = 1;
            }
        }
    }
    
    serial_print("PMM has finished Initializing!\n");
    volatile pmm_init_ = true;
    release(&pmm_lock);
}

// pmm alloc with bitmap
void *pmm_alloc(uint64_t size, bool zeroed)
{
    acquire(&pmm_lock);
    for (int i = 0; i < bitmap; i++)
    {
        // find a amount of pages that are free
        uint64_t free_pages = 0;
        for (int j = 0; j < size; j++)
        {
            if (bitmap[i + j] == 0)
            {
                free_pages++;
            }
        }
        if (free_pages == size)
        {
            // set the bitmap to 1
            for (int j = 0; j < size; j++)
            {
                bitmap[i + j] = 1;
                //serial_print("Allocated Page: ");
                //serial_print(to_string(i + j));
                //serial_print("\n");
            }
            // return the pointer
            if (zeroed)
            {
                memset((void*)(i * 4096), 0, size * 4096);
            }
            release(&pmm_lock);
            return (void*)(i * 4096);
        }
        
    }
    release(&pmm_lock);
    return NULL;
}
// pmm free with bitmap
void pmm_free(void *ptr, uint64_t size)
{
    acquire(&pmm_lock);
    uint64_t index = (uint64_t)ptr / 4096;
    // get size of the pointer
    for (int i = 0; i < size; i++)
    {
        bitmap[index + i] = 0;
        serial_print("Free Page: ");
        serial_print(to_string(index + i));
        serial_print(" Value: ");
        serial_print(to_string(bitmap[index + i]));
        serial_print("\n");
    }
    release(&pmm_lock);
}