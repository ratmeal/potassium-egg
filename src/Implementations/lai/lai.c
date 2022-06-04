#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "../../memory/vmm.h"
#include "../../serial/serial.h"
#include "../../memory/pmm.h"
#include "../../Drivers/Communcation/data.h"
// Functions for lai
/* Maps count bytes from the given physical address and returns
   a virtual address that can be used to access the memory. */
void *laihost_map(size_t address, size_t count) {
  // note: `count` is unused since we currently know that it does not go beyond the mapped range.
  return (void*)(address + higher_half);
}
void laihost_unmap(void *pointer, uint64_t count)
{
    // You're a waste of air you know that?
    // a Waste of Human Space
    // Kill Yourself.
    // If you're reading this I hope you commit lego step x1B
}
void laihost_log(int level, const char *msg)
{
    serial_print("lai: ");
    serial_print(msg);
}
void *laihost_malloc(size_t size)
{
    return malloc(size);
}
void *laihost_realloc(void *oldptr, size_t newsize, size_t oldsize)
{
    return realloc(oldptr, newsize);
}
void laihost_free(void *ptr, size_t size)
{
    free(ptr);
}
void laihost_outb(uint16_t port, uint8_t val)
{
    outb(port, val);
}
uint8_t laihost_inb(uint16_t port)
{
    return inb(port);
}
void laihost_outw(uint16_t port, uint16_t val)
{
    outw(port, val);
}
uint16_t laihost_inw(uint16_t port)
{
    return inw(port);
}
void laihost_outd(uint16_t port, uint32_t val)
{
    outd(port, val);
}
uint32_t laihost_ind(uint16_t port)
{
    return ind(port);
}
/* Returns the (virtual) address of the n-th table that has the given signature,
   or NULL when no such table was found. */
void *laihost_scan(char *sig, size_t index)
{
    
}