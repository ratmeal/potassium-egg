// PHOTON INTERNEL "Wave" Kernel
// Code Ported from VINIX (Copyright (C) 2021-2022, The VINIX Project)
// Code is governed by the GPL-2.0 license.
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "../klock/klock.h"
#include "../serial/serial.h"
#include "../../limine.h"
#include "../utils.h"
#include "pmm.h"
#include "../Graphics/graphics.h"
extern struct limine_hhdm_request hhdm_request;
static bool vmm_init_ = false;
const uint64_t pte_present = (uint64_t)1 << 0;
const uint64_t pte_writable = (uint64_t)1 << 1;
const uint64_t pte_user = (uint64_t)1 << 2;
uint64_t page_size = (uint64_t)0x1000;
uint64_t higher_half = (uint64_t)0xffff800000000000;

struct PageMap
{
    struct lock l;
    uint64_t *top_level;
    void *mmap_ranges[];
};
struct PageMap kernel_pagemap;
extern struct limine_memmap_request memmap_request;
extern struct limine_kernel_address_request kernel_address_request;
struct PageMap *new_pagemap()
{
    uint64_t *top_level = (uint64_t*)pmm_alloc(1, true);
    if (top_level == NULL)
    {
        serial_print("failed to allocate top level page table!\n");
        serial_print("kpanic!\n");
    }
    // Import higher half from kernel pagemap
    uint64_t *p1 = (uint64_t*)((uint64_t)top_level + higher_half);
    uint64_t *p2 = (uint64_t*)((uint64_t)kernel_pagemap.top_level + higher_half);
    for (uint64_t i = 256; i < 512; i++)
    {
        p1[i] = p2[i];
    }
    struct PageMap *pa;
    pa->top_level = top_level;
    
    return pa;
}

uint64_t *get_next_level(uint64_t *current_level, uint64_t index, bool allocate)
{
    uint64_t *ret = (uint64_t*)0;
    uint64_t *entry = (uint64_t*)((uint64_t)current_level + higher_half + index * 8);
    if (entry[0] & 0x01 != 0)
    {
        ret = (uint64_t*)(entry[0] & ~(uint64_t)0xfff);
    }
    else
    {
        if (allocate == false)
        {
            return NULL;
        }
        ret = pmm_alloc(1, true);
        if (ret == 0)
        {
            return NULL;
        }
        entry[0] = (uint64_t)ret | 0b111;
    }
    return ret;
}
uint64_t *virt2pte(struct PageMap *pagemap, uint64_t virt, bool allocate)
{
    uint64_t pml4_entry = (virt & ((uint64_t)0x1ff << 39)) >> 39;
    uint64_t pml3_entry = (virt & ((uint64_t)0x1ff << 30)) >> 30;
    uint64_t pml2_entry = (virt & ((uint64_t)0x1ff << 21)) >> 21;
    uint64_t pml1_entry = (virt & ((uint64_t)0x1ff << 12)) >> 12;

    uint64_t *pml4 = pagemap->top_level;
    uint64_t *pml3 = get_next_level(pml4, pml4_entry, allocate);
    if (pml3 == 0)
    {
        return NULL;
    }
    uint64_t *pml2 = get_next_level(pml3, pml3_entry, allocate);
    if (pml2 == 0)
    {
        return NULL;
    }
    uint64_t *pml1 = get_next_level(pml2, pml2_entry, allocate);
    if (pml1 == 0)
    {
        return NULL;
    }
    return (uint64_t*)((uint64_t)&pml1[pml1_entry] + higher_half);
}
uint64_t virt2phys(struct PageMap *pagemap, uint64_t virt)
{
    uint64_t *pte_p = virt2pte(pagemap, virt, false);
    if (pte_p == NULL)
    {
        return 0;
    }
    return pte_p[0] & ~(uint64_t)0xfff;
}
void switch_to(struct PageMap *pagemap)
{
    uint64_t *top_level = pagemap->top_level;
    __asm__ volatile ("mov %0, %%cr3" ::"r"(top_level) : "memory");
}
void unmap_page(struct PageMap *pagemap, uint64_t virt)
{
    uint64_t *pte_p = virt2pte(pagemap, virt, false);

    *pte_p = 0;
    uint64_t current_cr3 = read_cr3();
    if (current_cr3 == (uint64_t)pagemap->top_level)
    {
        invlpg(virt);
    }
}
void flag_page(struct PageMap *pagemap, uint64_t virt, uint64_t flags)
{
    uint64_t *pte_p = virt2pte(pagemap, virt, true);
    if (pte_p == NULL)
    {
        return "Failure";
    }
    *pte_p &= ~(uint64_t)0xfff;
    *pte_p |= flags;
    uint64_t current_cr3 = read_cr3();
    if (current_cr3 == (uint64_t)pagemap->top_level)
    {
        invlpg(virt);
    }
}
void map_page(struct PageMap *pagemap, uint64_t virt, uint64_t phys, uint64_t flags)
{
    acquire(pagemap->l);
    uint64_t pml4_entry = (virt & ((uint64_t)0x1ff << 39)) >> 39;
    uint64_t pml3_entry = (virt & ((uint64_t)0x1ff << 30)) >> 30;
    uint64_t pml2_entry = (virt & ((uint64_t)0x1ff << 21)) >> 21;
    uint64_t pml1_entry = (virt & ((uint64_t)0x1ff << 12)) >> 12;
    uint64_t *pml4 = pagemap->top_level;
    uint64_t *pml3 = get_next_level(pml4, pml4_entry, true);
    if (pml3 == 0)
    {
        return NULL;
    }
    uint64_t *pml2 = get_next_level(pml3, pml3_entry, true);
    if (pml2 == 0)
    {
        return NULL;
    }
    uint64_t *pml1 = get_next_level(pml2, pml2_entry, true);
    if (pml1 == 0)
    {
        return NULL;
    }
    uint64_t *entry = (uint64_t*)((uint64_t)pml1 + higher_half + pml1_entry * 8);
    entry[0] = phys | flags;
    release(pagemap->l);
}
void vmm_init()
{
    if (vmm_init_ == true)
    {
        serial_print("vmm_init() already called\n");
        return;
    }
    
    serial_print("Initializing VMM\n");
    serial_print("vmm: Kernel Physical Address: ");
    serial_print(to_hstring(kernel_address_request.response->physical_base));
    serial_print("\n");
    serial_print("vmm: Kernel Virtual Address: ");
    serial_print(to_hstring(kernel_address_request.response->virtual_base));
    serial_print("\n");
    
    // keep a direct map
    kernel_pagemap.top_level = pmm_alloc(1, true);
    if (kernel_pagemap.top_level == 0)
    {
        serial_print("vmm: Failed to allocate kernel page map!\n");
        serial_print("kpanic!\n");
        return;
    }
    // VINIX COMMENT:
    // Since the higher half has to be shared amongst all address spaces,
	// we need to initialise every single higher half PML3 so they can be
	// shared.
    for (uint64_t i = (uint64_t)256; i < 512; i++)
    {
        get_next_level(kernel_pagemap.top_level, i, true);
    }
    uint64_t virt = kernel_address_request.response->virtual_base;
    uint64_t phys = kernel_address_request.response->physical_base;
    uint64_t len = 0x10000000;
    serial_print("vmm: Mapping Kernel Physical Address 0x");
    serial_print(to_hstring(phys));
    serial_print(" to 0x");
    serial_print(to_hstring(virt));
    serial_print(" Virtual Address");
    serial_print("\nLength: 0x");
    serial_print(to_hstring(len));
    serial_print("\n");
    
    for (uint64_t j = (uint64_t)0; j < len; j += page_size)
    {
        map_page(&kernel_pagemap, virt + j, phys + j, 0x03);
    }
    for (uint64_t i = (uint64_t)0x1000; i < 0x100000000; i += page_size)
    {
        map_page(&kernel_pagemap, i, i, 0x03);
        map_page(&kernel_pagemap, i + higher_half, i, 0x03);
    }
    for (int i = 0; i < memmap_request.response->entry_count; i++)
    {
        uint64_t base = align_down(memmap_request.response->entries[i]->base, page_size);
        uint64_t top = align_up(memmap_request.response->entries[i]->base + memmap_request.response->entries[i]->length, page_size);
        if (top <= (uint64_t)0x100000000)
        {
            continue;
        }
        for (uint64_t j = base; j < top; j += page_size)
        {
            if (j < (uint64_t)0x100000000)
            {
                continue;
            }
            map_page(&kernel_pagemap, j, j, 0x03);
            map_page(&kernel_pagemap, j + higher_half, j, 0x03);
        }
    }
    switch_to(&kernel_pagemap);
    volatile vmm_init_ = true;
}
