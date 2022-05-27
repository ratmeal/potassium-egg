// PHOTON INTERNEL "Wave" Kernel
// Code is governed by the GPL-2.0 license.

#include <stdint.h>
// include serial.h
#include "serial.h"
#include "utils.h"
struct idt_pointer {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));
struct InterruptDescriptor64 {
   uint16_t offset_low;        // offset bits 0..15
   uint16_t selector;        // a code segment selector in GDT or LDT
   uint8_t  ist;             // bits 0..2 holds Interrupt Stack Table offset, rest of bits zero.
   uint8_t  type_attributes; // gate type, dpl, and p fields
   uint16_t offset_middle;        // offset bits 16..31
   uint32_t offset_high;        // offset bits 32..63
   uint32_t zero;            // reserved
};
void page_fault()
{
    // we dont have paging yet so we just print out the error
    serial_print("PAGE FAULT SOMEHOW\n");
    for (;;)
    {
        __asm__ volatile ("hlt");
    }
}
void keyboard_interrupt()
{
    serial_print("KEYBOARD LOL");
}
void divide_error()
{
    serial_print("DIVIDE ERROR LOL");
    kprintf_limine("DIVIDE ERROR!\n", sizeof("DIVIDE ERROR!\n"));
}

// wha
// add interrupt descriptor to idt
void add_idt_entry(uint8_t interrupt, void *handler, uint16_t selector, uint8_t flags, struct InterruptDescriptor64 idt[256])
{
     idt[interrupt].offset_low = (uint64_t)handler;
     idt[interrupt].selector = selector;
     idt[interrupt].ist = 0;
     idt[interrupt].type_attributes = flags;
     idt[interrupt].offset_middle = (uint64_t)handler >> 16;
     idt[interrupt].offset_high = (uint64_t)handler >> 32;
     idt[interrupt].zero = 0;
}
void init_idt(void)
{
    // idt
    struct InterruptDescriptor64 idt[256];
    // setup interrupts
    add_idt_entry(0, &divide_error, 0x28, 0x8E, idt);
    add_idt_entry(0x21, &keyboard_interrupt, 0x28, 0x8E, idt);
    // add page fault expection
    add_idt_entry(0x0E, &page_fault, 0x28, 0x8E, idt);
    // idt pointer
    struct idt_pointer idtp = {
        .limit = sizeof(idt) - 1,
        .base = (uint64_t)idt
    };
    // load
    __asm__ volatile ("lidt %0" :: "m" (idtp));
};
