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
#include "vmm.h"
extern struct limine_memmap_request memmap_request;
extern struct limine_hhdm_request hhdm_request;
struct lock pmm_lock;
uint8_t *bitmap;
static bool pmm_init_ = false;
uint64_t total_pages_global;
uint64_t free_pages_global = 0;
// VINIX SLAB ALLOCATOR CODE
struct Slab 
{
    struct lock l;
    uint64_t first_free;
    uint64_t ent_size;
};
struct SlabHeader
{
    struct Slab *slab;
};
struct Slab slabs[10];
struct MallocMetadata
{
    uint64_t pages;
    uint64_t size;
};
// END OF VINIX SLAB ALLOCATOR CODE
void pmm_init()
{
    
    if (pmm_init_ == true)
    {
        serial_print("oh no!\n");
        return;
    }
    // print out each entry in the memmap
    uint64_t highest_page_usable = 0;
    uint64_t last_page_last_entry = 0;
    for (uint64_t i = 0; i < memmap_request.response->entry_count; i++)
    {
        uint64_t last_page = memmap_request.response->entries[i]->base + memmap_request.response->entries[i]->length - page_size;
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
    uint64_t bitmap_size = (highest_page_usable / page_size) / 8;
    uint64_t bitmap_size_aligned = align_up(bitmap_size, page_size);
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
            for (uint64_t j = 0; j < memmap_request.response->entries[i]->length; j += page_size)
            {
                // get the index for the bitmap
                uint64_t index = j / page_size;
                
                bitmap[index] = 0;
                free_pages_global++;
                total_pages_global++;
            }
        }
        else
        {
            // loop through each page in the entry
            for (uint64_t j = 0; j < memmap_request.response->entries[i]->length; j += page_size)
            {
                // get the index for the bitmap
                uint64_t index = j / page_size;
                
                bitmap[index] = 1;
                total_pages_global++;
            }
        }
    }
    serial_print("Initing VINIX Slabs\n");
    // VINIX SLAB ALLOCATOR CODE
    slab_init(&slabs[0], 8);
    slab_init(&slabs[1], 16);
    slab_init(&slabs[2], 24);
    slab_init(&slabs[3], 32);
    slab_init(&slabs[4], 48);
    slab_init(&slabs[5], 64);
    slab_init(&slabs[6], 128);
    slab_init(&slabs[7], 256);
    slab_init(&slabs[8], 512);
    slab_init(&slabs[9], 1024);
    // END OF VINIX SLAB ALLOCATOR CODE
    serial_print("Done!\n");
    serial_print("PMM has finished Initializing!\n");
    volatile pmm_init_ = true;
}

// pmm alloc with bitmap
void *pmm_alloc(uint64_t size, bool zeroed)
{
    acquire(pmm_lock);
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
                free_pages_global--;
                //serial_print("Allocated Page: ");
                //serial_print(to_string(i + j));
                //serial_print("\n");
            }
            // return the pointer
            if (zeroed)
            {
                memset((void*)(i * page_size), 0, size * page_size);
            }
            release(pmm_lock);
            return (void*)(i * page_size);
        }
        
    }
    release(pmm_lock);
    return NULL;
}
// pmm free with bitmap
void pmm_free(void *ptr, uint64_t size)
{
    acquire(pmm_lock);
    uint64_t index = (uint64_t)ptr / page_size;
    // get size of the pointer
    for (int i = 0; i < size; i++)
    {
        bitmap[index + i] = 0;
        free_pages_global++;
    }
    release(pmm_lock);
}
// VINIX SLAB ALLOCATOR CODE
void slab_init(struct Slab *slab, uint64_t ent_size)
{
    slab->ent_size = ent_size;
    slab->first_free = (uint64_t)pmm_alloc(1, false);
    slab->first_free += higher_half;
    uint64_t avl_size = page_size - align_up(sizeof(struct SlabHeader), ent_size);
    struct SlabHeader *slabptr = (struct SlabHeader*)slab->first_free;
    slabptr[0].slab = slab;
    slab->first_free += align_up(sizeof(struct SlabHeader), ent_size);
    uint64_t *arr = (uint64_t*)slab->first_free;
    uint64_t max = avl_size / ent_size - 1;
    uint64_t fact = ent_size / 8;
    for (uint64_t i = 0; i < max; i++)
    {
        arr[i * fact] = (uint64_t)&arr[(i + 1) * fact];
    }
    arr[max * fact] = (uint64_t)0;
}
void *slab_alloc(struct Slab *slab)
{
    acquire(slab->l);
    if (slab->first_free == 0)
    {
        slab_init(&slab, slab->ent_size);
    }
    uint64_t *old_free = (uint64_t*)slab->first_free;
    slab->first_free = old_free[0];
    memset((void*)old_free, 0, slab->ent_size);
    release(slab->l);
    return (void*)old_free;
}
void slab_sfree(struct Slab *slab, void *ptr)
{
    acquire(slab->l);
    if (ptr == (void*)0)
    {
        release(slab->l);
        return;
    }
    uint64_t *new_head = (uint64_t*)ptr;
    new_head[0]  = slab->first_free;
    slab->first_free = (uint64_t)new_head;
    release(slab->l);
}
void big_free(void *ptr)
{
    struct MallocMetadata *metadata = (uint64_t)ptr - page_size;
    pmm_free((void*)((uint64_t)metadata - higher_half), metadata->pages + 1);
}
// Make this function also in the header file
void free(void *ptr)
{
    if (ptr == (void*)0)
    {
        return;
    }
    if ((uint64_t)ptr & (uint64_t)0xfff == 0)
    {
        big_free(ptr);
        return;
    }
    struct SlabHeader *slab_hdr = (uint64_t)ptr & ~(uint64_t)0xfff;
    slab_sfree(slab_hdr->slab, ptr);
}

struct Slab *slab_for(uint64_t size)
{
    struct Slab *ptr = slabs;
    for (int i = 0; i < 10; i++, ptr++)
    {
        if (ptr->ent_size >= size)
        {
            return ptr;
        }
    }
    struct Slab *pa;
    pa->ent_size = 0;
    return pa;
}
void *big_alloc(uint64_t size)
{
    uint64_t page_count = div_roundup(size, page_size);
    void *ptr = pmm_alloc(page_count + 1, true);
    if (ptr == 0)
    {
        return 0;
    }
    struct MallocMetadata *metadata = (uint64_t)ptr + higher_half;
    metadata->pages = page_count;
    metadata->size = size;
    return (void*)(uint64_t)ptr + higher_half + page_size;
}
// Make this function also in the header file
void *malloc(uint64_t size)
{
    struct Slab *slab = slab_for(8 + size);
    serial_print("Malloc called\n"); // dummy line otherwise page write fault, dunno why
    if (slab->ent_size == 0)
    {
        return big_alloc(size);
    }
    return slab_alloc(slab);
}
void *big_realloc(void *ptr, uint64_t new_size)
{
    struct MallocMetadata *metadata = (uint64_t)ptr - page_size;
    if (div_roundup(metadata->size, page_size) == div_roundup(new_size, page_size))
    {
        metadata->size = new_size;
        return ptr;
    }
    void *new_ptr = malloc(new_size);
    if (new_ptr == 0)
    {
        return 0;
    }
    if (metadata->size > new_size)
    {
        memcpy(new_ptr, ptr, new_size);
    }
    else
    {
        memcpy(new_ptr, ptr, metadata->size);
    }
    free(ptr);
    return new_ptr;
}
// Make this function also in the header file
void *realloc(void *ptr, uint64_t new_size)
{
    if (ptr == 0)
    {
        return malloc(new_size);
    }
    if ((uint64_t)ptr & (uint64_t)0xfff == 0)
    {
        
        return big_realloc(ptr, new_size);
    }
    struct SlabHeader *slab_hdr = (uint64_t)ptr & ~(uint64_t)0xfff;
    struct Slab *slab = slab_hdr->slab;
    if (new_size > slab->ent_size)
    {
        void *new_ptr = malloc(new_size);
        memcpy(new_ptr, ptr, slab->ent_size);
        slab_sfree(slab, ptr);
        return new_ptr;
    }
    return ptr;
}
void *calloc(uint64_t a, uint64_t b)
{
    return malloc(a * b);
}
// END OF VINIX SLAB ALLOCATOR CODE
uint64_t available_memory()
{
    return free_pages_global * page_size;
}
uint64_t total_memory()
{
    return total_pages_global * page_size;
}