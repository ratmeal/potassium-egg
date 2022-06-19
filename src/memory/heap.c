// PHOTON INTERNEL "Wave" Kernel
// Code is governed by the GPL-2.0 license.

// TARGET FUNCTIONS TO IMPLMENT:
//void free(void *ptr);
//void *malloc(uint64_t size);
//void *realloc(void *ptr, uint64_t new_size);
//void *calloc(uint64_t a, uint64_t b);


#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "../klock/klock.h"
#include "../serial/serial.h"
#include <limine.h>
#include "../utils.h"
#include "vmm.h"
#include "pmm.h"
extern struct limine_hhdm_request hhdm_request;
struct Slab
{
    void *baseaddr;
    uint8_t *bitmap;
    uint64_t obj_size;
    uint64_t obj_count;
    uint64_t used_objs;
    struct lock l;
    struct Slab *next;
};
static struct Slab *slabs;
void slab_init(uint64_t size)
{
    struct Slab *cur_slab;
    if (!slabs)
    {
        slabs = (pmm_calloc(align_up(sizeof(struct Slab), page_size) / page_size) + hhdm_request.response->offset);
        cur_slab = slabs;
    }
    else
    {
        cur_slab = slabs;
        while (slabs->next)
        {
            cur_slab = cur_slab->next;
        }
        cur_slab->next = (pmm_calloc(align_up(sizeof(struct Slab), page_size) / page_size) + hhdm_request.response->offset);
        cur_slab = cur_slab->next;
    }
    if (cur_slab == NULL)
    {
        // kpanic
        return;
    }
    cur_slab->obj_count = 4096;
    cur_slab->obj_size = size;
    cur_slab->baseaddr = (pmm_alloc(align_up(cur_slab->obj_count * cur_slab->obj_size, page_size) / page_size, true) + hhdm_request.response->offset);
    cur_slab->bitmap = (pmm_calloc(align_up(cur_slab->obj_count / 8, page_size) / page_size) + hhdm_request.response->offset);
    cur_slab->used_objs = 0;
    cur_slab->l.locked = false;
    if (cur_slab == NULL || cur_slab->bitmap == NULL)
    {
        // kpanic
        return;
    }
}
void heap_init()
{
    slab_init(8);
}
void *malloc(uint64_t size)
{
    if (size % page_size == 0)
    {
        return pmm_alloc((size / page_size), true);
    }
    alloc:
        if (!slabs)
        {
            slab_init(size);
        }
        struct Slab *cur_slab = slabs;
        acquire(&cur_slab->l);
        while (cur_slab)
        {
            if (cur_slab->obj_size == size)
            {
                for (uint64_t i = 0; i < cur_slab->obj_size; i++)
                {
                    if (cur_slab->bitmap[i] == 0)
                    {
                        cur_slab->bitmap[i] = 1;
                        cur_slab->used_objs++;
                        release(&cur_slab->l);
                        return cur_slab->baseaddr + (i * cur_slab->obj_size);
                    }
                }
            }
            release(&cur_slab->l);
            cur_slab = cur_slab->next;
        }
        slab_init(size);
        goto alloc;
}
void free(void *ptr)
{
    struct Slab *cur_slab = slabs;
    while (cur_slab)
    {
        acquire(&cur_slab->l);
        if (ptr >= cur_slab->baseaddr && ptr < cur_slab->baseaddr + (cur_slab->obj_count * cur_slab->obj_size))
        {
            uint64_t index = (ptr - cur_slab->baseaddr) / cur_slab->obj_size;
            cur_slab->obj_count--;
            cur_slab->bitmap[index] = 0;
            release(&cur_slab->l);
            return;
        }
        release(&cur_slab->l);
        cur_slab = cur_slab->next;
    }
    // panic here ig
}
void *realloc(void *ptr, uint64_t new_size, uint64_t old_size)
{
    if (ptr == (void *)0)
    {
        return malloc(new_size);
    }
    if (new_size == 0)
    {
        free(ptr);
        return;
    }
    void *ret = malloc(new_size);
    memcpy(ret, ptr, old_size);
    free(ptr);
    return ret;
}
void *calloc(uint64_t a, uint64_t b)
{
    if (a * b % page_size == 0)
    {
        return pmm_calloc((a * b / page_size));
    }
    void *ret = malloc(a * b);
    memset(ret, 0, a * b);
    return ret;
}
