#include "../limine.h"
static uint64_t gdt[] = {0x0000000000000000, 0x00009a000000ffff, 0x000093000000ffff, 0x00cf9a000000ffff, 0x00cf93000000ffff, 0x00af9b000000ffff, 0x00af93000000ffff, 0x00affb000000ffff, 0x00aff3000000ffff};

struct gdtr_t {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));
// This function will initialize the GDT and return the GDT Entries.
uint64_t init_gdt(void) 
{
    
    struct gdtr_t gdtr = {sizeof(gdt) - 1, (uint64_t)gdt};
    __asm__ volatile ("lgdt %0" :: "m" (gdtr));
    return gdt;
};