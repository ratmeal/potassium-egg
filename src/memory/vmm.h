#ifndef VMM.h
#define VMM.h
#include "../klock/klock.h"
void vmm_init();
extern struct PageMap kernel_pagemap;
extern uint64_t page_size;
uint64_t read_msr(uint64_t msr);
void write_msr(uint64_t msr, uint64_t value);
struct PageMap *new_pagemap();
uint64_t *get_next_level(uint64_t *current_level, uint64_t index, bool allocate);
uint64_t *virt2pte(struct PageMap *pagemap, uint64_t virt, bool allocate);
uint64_t virt2phys(struct PageMap *pagemap, uint64_t virt);
void switch_to(struct PageMap *pagemap);
void flag_page(struct PageMap *pagemap, uint64_t virt, uint64_t flags);
void unmap_page(struct PageMap *pagemap, uint64_t virt);
void map_page(struct PageMap *pagemap, uint64_t virt, uint64_t phys, uint64_t flags);
#endif