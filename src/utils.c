// PHOTON INTERNEL "Wave" Kernel
// Code is governed by the GPL-2.0 license.

#include <stdint.h>
#include <stddef.h>
#include <limine.h>
#include <stdbool.h>
// dec to string
void dec_to_str(uint64_t dec, char* str) 
{
    str[sizeof(str) - 1] = '\0';
    for (int i = 18; i >= 0; i--) {
       str[i] = dec % 10 + 48;
       dec /= 10;
    }
};
// hex to string
void hex_to_str(uint64_t hex, char* str) 
{
    str[sizeof(str) - 1] = '\0';
    for (int i = 15; i >= 0; i--) {
       str[i] = hex % 16 + 48;
       hex /= 16;
    }
};
struct limine_terminal_request terminal_request;
struct limine_terminal* terminal;
void kprintf_limine(char* str, unsigned long length) 
{
    terminal_request.response->write(terminal, str, length);
};
// set limine terminal without optimizing
void set_limine_terminal(struct limine_terminal_request volatile request, struct limine_terminal* volatile terminal) 
{
    terminal_request = request;
    terminal = terminal;
};
// div_roundup for all types
uint64_t div_roundup(uint64_t a, uint64_t b)
{
    return (a + (b - 1)) / b;
}
uint64_t align_up(uint64_t value, uint64_t alignment)
{
    return div_roundup(value, alignment) * alignment;
};
void *memset(void *dest, int c, size_t size)
{
    uint8_t *destm = (uint8_t *)dest;

    for (int i = 0; i < size; i++)
    {
        destm[i] = (uint8_t)c;
    }
    return dest;
}
void bitreset(void *bitmap, uint64_t index)
{
    uint64_t *fbitmap = (uint64_t)&bitmap;
    uint64_t bits_type = sizeof(uint64_t) * 8;
    uint64_t test_index = index % bits_type;
    fbitmap[index / bits_type] &= ~((uint64_t)1 << test_index);
}
bool bittest(void *bitmap, uint64_t index)
{
    uint64_t *fbitmap = (uint64_t)&bitmap;
    uint64_t bits_type = sizeof(uint64_t) * 8;
    uint64_t test_index = index % bits_type;
    uint64_t test_sample = fbitmap[index / bits_type];
    return ((test_sample >> test_index) & (uint64_t)1) != 0;
}
void bitset(void *bitmap, uint64_t index)
{
    uint64_t *fbitmap = (uint64_t)&bitmap;
    uint64_t bits_type = sizeof(uint64_t) * 8;
    uint64_t test_index = index % bits_type;
    fbitmap[index / bits_type] |= (uint64_t)1 << test_index;
}
void *memcpy(void *dest, void *src, uint64_t size)
{
    uint8_t *destm = (uint8_t *)dest;
    uint8_t *srcm = (uint8_t *)src;
    for (int i = 0; i < size; i++)
    {
        destm[i] = srcm[i];
    }
    return dest;
}
uint64_t read_cr3()
{
    uint64_t cr3;
    __asm__ volatile ("mov %%cr3, %0" : "=r"(cr3) :: "memory");
    return cr3;
}
void invlpg(uint64_t addr)
{
    
    __asm__ volatile ("invlpg (%0)" :: "r"(addr) : "memory");
}
uint64_t align_down(uint64_t value, uint64_t alignment)
{
    return (value / alignment) * alignment;
}