#ifndef PMM.h
#define PMM.h
void pmm_init();
void *pmm_alloc(uint64_t size, bool zeroed);
void pmm_free(void *ptr, uint64_t size);
uint64_t available_memory();
void free(void *ptr);
void *malloc(uint64_t size);
void *realloc(void *ptr, uint64_t new_size);
void *calloc(uint64_t a, uint64_t b);
uint64_t total_memory();
#endif