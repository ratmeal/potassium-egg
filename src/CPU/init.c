#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <limine.h>
#include "../utils.h"
#include "../serial/serial.h"
#include "../memory/vmm.h"
#include "../GDT/gdt.h"
#include "../Interrupts/interrupts.h"
void enable_pat()
{
    uint64_t pat_msr = read_msr(0x277);
    pat_msr &= 0xffffffff;
    pat_msr |= (uint64_t)0x0105 << 32;
    write_msr(0x277, pat_msr);
    serial_print("PAT enabled\n");
}
void init_cpu(struct limine_smp_info *cpu)
{
    init_gdt();
    init_idt();
    switch_to(&kernel_pagemap);
}