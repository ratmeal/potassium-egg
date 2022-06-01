// PHOTON INTERNEL "Wave" Kernel
// Code is governed by the GPL-2.0 license.
#include <stdint.h>
#include <stddef.h>
#include <limine.h>
#include "src/utils.h"
#include "src/GDT/gdt.h"
#include "src/serial/serial.h"
#include "src/Interrupts/interrupts.h"
#include "src/memory/pmm.h"
#include "src/memory/vmm.h"
 
// The Limine requests can be placed anywhere, but it is important that
// the compiler does not optimise them away, so, usually, they should
// be made volatile or equivalent.
// extern void fill_screen(uint64_t color);
// extern void graphics_init();
static void done(void) {
    for (;;) {
        __asm__("hlt");
    }
};
extern void draw_pixel(uint64_t x, uint64_t y, uint64_t color);
extern void fill_screen(uint64_t color);
extern void kpanic(const char *message, size_t message_size);
// The following will be our kernel's entry point.
void _start(void) {
    if (init_serial() != 0) {
        done();
    }
    else {
        serial_print("Hello World!\n");
    };
    // We should now be able to call the Limine terminal to print out
    // a simple "Hello World" to screen.
    uint64_t gdt = init_gdt();
    serial_print("GDT loaded\n");
    init_idt();
    serial_print("IDT loaded\n");
    pmm_init();
    serial_print("PMM loaded\n");
    // free memory in kilobytes
    //kpanic("don don", sizeof("don don"));
    //graphics_init();
    //fill_screen(0x018281);
    // read serial
    serial_print("moment of truth rayan\n");
    vmm_init();
    serial_print("We Did it!\n");
    done();
};

