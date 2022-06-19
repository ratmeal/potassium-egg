#ifndef HEAP.h
#define HEAP.h
void free(void *ptr);
void *malloc(uint64_t size);
void *realloc(void *ptr, uint64_t new_size, uint64_t old_size);
void *calloc(uint64_t a, uint64_t b);
#endif