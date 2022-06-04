// PHOTON INTERNEL "Wave" Kernel
// Code is governed by the GPL-2.0 license.

#include <stdint.h>
// include serial.h
#include "../serial/serial.h"
#include "../utils.h"
struct idt_pointer {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));
extern void page_fault_asm();
extern void divide_by_zero_asm();
extern void bound_range_asm();
extern void double_fault_asm();
struct InterruptDescriptor64 {
   uint16_t offset_low;        // offset bits 0..15
   uint16_t selector;        // a code segment selector in GDT or LDT
   uint8_t  ist;             // bits 0..2 holds Interrupt Stack Table offset, rest of bits zero.
   uint8_t  type_attributes; // gate type, dpl, and p fields
   uint16_t offset_middle;        // offset bits 16..31
   uint32_t offset_high;        // offset bits 32..63
   uint32_t zero;            // reserved
};
struct cpu_state_error {
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rbp;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rax;
    uint64_t error;
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
};
struct cpu_state {
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rbp;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rax;
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
};
void page_fault(struct cpu_state_error *cpu)
{
    // we dont have paging yet so we just print out the error
    serial_print("PAGE FAULT SOMEHOW\n");
    serial_print("Error: ");
    // PRINT OUT ERROR CODE HERE
    serial_print(to_string(cpu->error));
    serial_print("\n");
    serial_print("sorry about that, we'll try to continue\n");
    for (;;)
    {
        __asm__ volatile ("hlt");
    }
}
void bound_range_exceeded(struct cpu_state *cpu)
{
    serial_print("bound exceeded don't do that again\n");
}
void divide_error(struct cpu_state *cpu)
{
    if ((cpu->cs & 0x3) == 3)
    {
        serial_print("yes your from user mode\n");
        // we'd kill the thread that caused this with SIGFPE
        
    }
    else
    {
        serial_print("no your from kernel mode\n");
        // panic here
    }
}
void double_fault_handler(struct cpu_state_error *cpu)
{
    serial_print("DOUBLE FAULT\n");
    serial_print("sorry about that\n");
    for (;;)
    {
        __asm__ volatile ("hlt");
    }
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
    add_idt_entry(0x0, &divide_by_zero_asm, 0x28, 0x8E, idt);
    // add page fault expection
    add_idt_entry(0x0E, &page_fault_asm, 0x28, 0x8E, idt);
    // double fault
    add_idt_entry(0x08, &double_fault_asm, 0x28, 0x8E, idt);
    add_idt_entry(0x05, &bound_range_asm, 0x28, 0x8E, idt);
    // idt pointer
    struct idt_pointer idtp = {
        .limit = sizeof(idt) - 1,
        .base = (uint64_t)idt
    };
    // load
    __asm__ volatile ("lidt %0" :: "m" (idtp));
};
