#ifndef PMM.h
#define PMM.h
void pmm_init();
void *pmm_alloc(uint64_t size, bool zeroed);
void pmm_free(void *ptr, uint64_t size);
#endif