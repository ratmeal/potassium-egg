#ifndef UTILS.h
#define UTILS.h
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
uint64_t align_up(uint64_t value, uint64_t alignment);
void *memset(void *dest, int c, uint64_t size);
void bitreset(void *bitmap, uint64_t index);
bool bittest(void *bitmap, uint64_t index);
void bitset(void *bitmap, uint64_t index);
uint64_t div_roundup(uint64_t a, uint64_t b);
void *memcpy(void *dest, void *src, uint64_t size);
uint64_t read_cr3();
void invlpg(uint64_t addr);
uint64_t align_down(uint64_t value, uint64_t alignment);
void *memcpy32(void *dest, void *src, uint64_t size);
void *memmove(void *dest, const void *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);
const char *to_string(int64_t value);
const char* to_hstring(uint64_t value);
#endif