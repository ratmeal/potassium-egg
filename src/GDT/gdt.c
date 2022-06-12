#include "../../limine.h"
static uint64_t gdt[] = {0x0000000000000000, 0x00009a000000ffff, 0x000093000000ffff, 0x00cf9a000000ffff, 0x00cf93000000ffff, 0x00af9b000000ffff, 0x00af93000000ffff, 0x00affb000000ffff, 0x00aff3000000ffff};

struct gdtr_t {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));
// This makes me wanna kill myself
struct tss_t
{
    uint32_t reserved0;
    uint64_t RSP0;
    uint64_t RSP1;
    uint64_t RSP2;

    uint32_t reserved1;
    uint32_t reserved2;

    uint64_t IST1;
    uint64_t IST2;
    uint64_t IST3;
    uint64_t IST4;
    uint64_t IST5;
    uint64_t IST6;
    uint64_t IST7;
    
    uint32_t reserved3;
    uint32_t reserved4;

    uint16_t IOBP;
};
struct tss_desc
{
    uint16_t size;
    uint16_t base0;
    uint8_t  base1;
    uint8_t  access;
    uint8_t  flags;
    uint8_t  base2;
    uint32_t base3;
    uint32_t reserved;
};
static struct tss_desc tss_d;
static struct tss_t tss;
void load_tss(uint64_t tss_addr) {
    __asm__("ltr %%ax" ::"a"(tss_addr));
}
void set_tss_entry(uint64_t base, uint8_t flags, uint8_t access)
{
    tss_d.size = 104;
    tss_d.base0 = base & 0xFFFF;
    tss_d.base1 = (base >> 16) & 0xFF;
    tss_d.access = access;
    tss_d.flags = flags;
    tss_d.base2 = (base >> 24) & 0xFF;
    tss_d.base3 = (base >> 32);
    tss_d.reserved = 0;
}
// this probs causes a page fault idfk
void init_tss(uint64_t stack)
{
    set_tss_entry((uintptr_t)&tss, 0x20, 0x89);
    memset((void *)&tss, 0, sizeof(struct tss_t));

    tss.RSP0 = stack;
    tss.IST1 = 0; 
}
// This function will initialize the GDT and return the GDT Entries.
// init gdt would have a uint64_t stack parameter, but not in the mood for smp rn
uint64_t init_gdt(void) 
{
    // not in the mood for smp rn so we gonna comment out init
    // init_tss(stack); 
    struct gdtr_t gdtr = {sizeof(gdt) - 1, (uint64_t)gdt};
    
    __asm__ volatile ("lgdt %0" :: "m" (gdtr));
    return gdt;
};