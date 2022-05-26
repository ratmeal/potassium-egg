// PHOTON INTERNEL "Wave" Kernel
// CODE PORTED FROM VINIX Copyright (C) 2021-2022 The Vinix authors.
// Code is governed by the GPL-2.0 license.


#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "serial.h"
#include "utils.h"
#include "klock.h"
#include "../limine.h"
#include "vmm.h"
#include <stdatomic.h>
// shut up it didn't work in the .h file so I'm putting it here

struct lock pmm_lock;
void *pmm_bitmap = 0;
uint64_t pmm_avl_page_count = 0;
uint64_t pmm_last_used_index = 0;
uint64_t free_pages = 0;
struct limine_memmap_request memmap_req = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};
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
        serial_print("MEMMAP Entry\n");
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
    serial_print("BITMAP Calulated!\n");
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
};
void *inner_alloc(uint64_t count, uint64_t limit)
{
    int p = 0;
    while (pmm_last_used_index < limit)
    {
        if (!bittest(pmm_bitmap, pmm_last_used_index))
        {

            pmm_last_used_index++;
            p++;
            if ((uint64_t)p == count)
            {
                
                uint64_t page = pmm_last_used_index - count;
                for (uint64_t i = page; i < pmm_last_used_index; i++)
                {
                    bitset(pmm_bitmap, i);
                }
                
                return (void*)(page * page_size);
            }
            
            
        }
        else
        {
            pmm_last_used_index++;
            p = 0;
            
        }
    }
    return 0;
}

void *pmm_alloc_nozero(uint64_t count)
{
    acquire(&pmm_lock);
    uint64_t last = pmm_last_used_index;
    void *ret = inner_alloc(count, pmm_avl_page_count);
    if (&ret == 0)
    {
        pmm_last_used_index = 0;
        ret = inner_alloc(count, last);
        if (&ret == 0)
        {
            serial_print("out of memory oh noes!!!!\n");
            // panic here ig
        }
    }
    free_pages -= count;
    release(&pmm_lock);
    return ret;
}
void *pmm_alloc(uint64_t count)
{
    void *ret = pmm_alloc_nozero(count);
    uint64_t *ptr = (uint64_t*)((uint64_t)ret + higher_half);
    for (uint64_t i = 0; i < (count * page_size) / 8; i++)
    {
        ptr[i] = 0;
    }
    return ret;
}
void pmm_free(void *ptr, uint64_t count)
{
    acquire(&pmm_lock);
    uint64_t page = (uint64_t)ptr / page_size;
    for (uint64_t i = page; i < page + count; i++)
    {
        bitreset(pmm_bitmap, 1);
    }
    free_pages += count;
}

void slab_init(struct Slab *slab, uint64_t ent_size)
{
    slab->ent_size = ent_size;
    slab->first_free = (uint64_t)pmm_alloc_nozero(1);
    slab->first_free += higher_half;
    uint64_t avl_size = page_size - align_up(sizeof(struct SlabHeader), slab->ent_size);
    struct SlabHeader *slabptr = (struct SlabHeader*)slab->first_free;
    slabptr[0].slab = slab;
    slab->first_free += align_up(sizeof(struct SlabHeader), slab->ent_size);
    uint64_t *arr = (uint64_t*)slab->first_free;
    uint64_t max = avl_size / ent_size - 1;
    uint64_t fact = ent_size / 8;
    for (uint64_t i = 0; i < max; i++)
    {
        arr[i * fact] = (uint64_t)&arr[(i + 1) * fact];
    }
    arr[max * fact] = (uint64_t)0;
}
void *slab_alloc(struct Slab slab)
{
    acquire(&slab.l);
    if (slab.first_free == 0)
    {
        slab_init(&slab, slab.ent_size);
    }
    uint64_t *old_free = (uint64_t*)slab.first_free;
    slab.first_free = old_free[0];
    memset((void*)old_free, 0, slab.ent_size);
    release(&slab.l);
    return (void*)old_free;
}
void slab_sfree(struct Slab *slab, void *ptr)
{
    acquire(&slab->l);
    if (ptr == (void*)0)
    {
        release(&slab->l);
        return;
    }
    uint64_t *new_head = (uint64_t*)ptr;
    new_head[0] = slab->first_free;
    slab->first_free = (uint64_t)new_head;
    release(&slab->l);
}
void big_free(void *ptr)
{
    struct MallocMetadata *metadata = (uint64_t)ptr - page_size;
    pmm_free((void*)(uint64_t)metadata - higher_half, metadata->pages + 1);
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

struct Slab slab_for(uint64_t size)
{
    for (int i = 0; i < slabs; i++)
    {
        if (slabs[i].ent_size >= size)
        {
            return slabs[i];
        }
    }
    return;
}
void *big_alloc(uint64_t size)
{
    uint64_t page_count = div_roundup(size, page_size);
    void *ptr = pmm_alloc(page_count + 1);
    if (ptr == 0)
    {
        return 0;
    }
    struct MallocMetadata *metadata = (uint64_t)ptr - higher_half;
    metadata->pages = page_count;
    metadata->size = size;
    return (void*)(uint64_t)ptr + higher_half + page_size;
}
// Make this function also in the header file
void *malloc(uint64_t size)
{
    struct Slab slab = slab_for(8 + size);
    if (slab.ent_size == 0)
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
    }
}
void *calloc(uint64_t a, uint64_t b)
{
    return malloc(a * b);
}