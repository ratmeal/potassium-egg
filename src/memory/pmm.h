#ifndef PMM.h
#define PMM.h
void pmm_init();
void *pmm_alloc(uint64_t size, bool zeroed);
void pmm_free(void *ptr, uint64_t size);
void *pmm_calloc(uint64_t pages);
uint64_t available_memory();
uint64_t total_memory();
#endif