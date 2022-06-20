// PHOTON INTERNEL "Wave" Kernel
// Code is governed by the GPL-2.0 license.

#include <stdint.h>
#include <stddef.h>
#include <limine.h>
#include <stdbool.h>
#include "serial/serial.h"
// implmentation of to_string is borrowed from a poncho tutorial
// strcmp function
// memmove function
// memcmp function
int memcmp(const void *s1, const void *s2, size_t n)
{
    const uint8_t *p1 = s1;
    const uint8_t *p2 = s2;
    for (size_t i = 0; i < n; i++)
    {
        if (p1[i] != p2[i])
        {
            return p1[i] - p2[i];
        }
    }
    return 0;
}
void *memmove(void *dest, const void *src, size_t n)
{
    char *d = dest;
    const char *s = src;
    if (d <= s)
    {
        while (n--)
        {
            *d++ = *s++;
        }
    }
    else
    {
        d += n;
        s += n;
        while (n--)
        {
            *--d = *--s;
        }
    }
    return dest;
}
char intstringoutput[128];
const char *to_string(int64_t value)
{
    uint8_t neg = 0;
    if (value < 0)
    {
        neg = 1;
        value *= -1;
        intstringoutput[0] = '-';
    }
    uint8_t size;
    uint64_t sizetest = value;
    while (sizetest / 10 > 0)
    {
        sizetest /= 10;
        size++;
    }
    uint8_t index = 0;
    while (value / 10 > 0)
    {
        uint8_t remainder = value % 10;
        value /= 10;
        intstringoutput[neg + size - index] = remainder + '0';
        index++;
    }
    uint8_t remainder = value % 10;
    intstringoutput[neg + size - index] = remainder + '0';
    intstringoutput[neg + size + 1] = 0;
    return intstringoutput;
}
char hexTo_StringOutput[128];
const char* to_hstring(uint64_t value){
    uint64_t* valPtr = &value;
    uint8_t* ptr;
    uint8_t tmp;
    uint8_t size = 8 * 2 - 1;
    for (uint8_t i = 0; i < size; i++){
        ptr = ((uint8_t*)valPtr + i);
        tmp = ((*ptr & 0xF0) >> 4);
        hexTo_StringOutput[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');
        tmp = ((*ptr & 0x0F));
        hexTo_StringOutput[size - (i * 2)] = tmp + (tmp > 9 ? 55 : '0');
    }
    hexTo_StringOutput[size + 1] = 0;
    return hexTo_StringOutput;
}


// div_roundup for all types
uint64_t div_roundup(uint64_t a, uint64_t b)
{
    return (a + (b - 1)) / b;
}
uint64_t align_up(uint64_t value, uint64_t alignment)
{
    return div_roundup(value, alignment) * alignment;
};
void *memset (void *dest, const uint8_t val, size_t len)
{
    
  uint8_t *ptr = (uint8_t*)dest;
  
  while (len--)
  {
    *ptr++ = val;
  }

  return dest;
}

void *memcpy(void *dest, void *src, uint64_t size)
{
    uint8_t *destm = (uint8_t*)dest;
    uint8_t *srcm = (uint8_t*)src;
    for (uint64_t i = 0; i < size; i++)
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
uint64_t read_cr2()
{
    uint64_t cr2;
    __asm__ volatile ("mov %%cr2, %0" : "=r"(cr2) :: "memory");
    return cr2;
}
void invlpg(uint64_t addr)
{
    
    __asm__ volatile ("invlpg (%0)" :: "r"(addr) : "memory");
}
uint64_t align_down(uint64_t value, uint64_t alignment)
{
    return (value / alignment) * alignment;
}
// memcpy32
void memcpy32(uint32_t *dest, const uint32_t *src, uint64_t len)
{
	for (; len != 0; len--)
	{
		*dest = *src;

		dest++;
		src++;
	}
}