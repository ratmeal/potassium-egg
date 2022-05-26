void print_free();
void pmm_init();
void free(void *ptr);
void *malloc(uint64_t size);
void *realloc(void *ptr, uint64_t new_size);
void *calloc(uint64_t a, uint64_t b);
void *pmm_alloc(uint64_t count);