// PHOTON INTERNEL "Wave" Kernel
// Code is governed by the GPL-2.0 license.
#include <stdint.h>
#include <stddef.h>
#include <limine.h>
#include "src/utils.h"
#include "src/gdt.h"
#include "src/serial.h"
#include "src/interrupts.h"
#include "src/pmm.h"
 
// The Limine requests can be placed anywhere, but it is important that
// the compiler does not optimise them away, so, usually, they should
// be made volatile or equivalent.
 
static volatile struct limine_terminal_request terminal_request = {
    .id = LIMINE_TERMINAL_REQUEST,
    .revision = 0
};


static void done(void) {
    for (;;) {
        __asm__("hlt");
    }
};



// The following will be our kernel's entry point.
void _start(void) {
    if (init_serial() != 0) {
        done();
    }
    else {
        serial_print("Hello World!\n");
    };
    // Ensure we got a terminal
    if (terminal_request.response == NULL
     || terminal_request.response->terminal_count < 1) {
        done();
    }
    // We should now be able to call the Limine terminal to print out
    // a simple "Hello World" to screen.
    struct limine_terminal *terminal = terminal_request.response->terminals[0];
    set_limine_terminal(terminal_request, terminal);
    kprintf_limine("Photon Kernel 'Wave' is \e[0;32mLoading...\e[0m \n", sizeof("Photon Kernel 'Wave' is \e[0;32mLoading...\e[0m \n"));
    kprintf_limine("Please Standby... \n", sizeof("Please Standby... \n"));
    uint64_t gdt = init_gdt();
    kprintf_limine("[Wave Kernel] GDT Has Loaded!\n", sizeof("[Wave Kernel] GDT Has Loaded!\n"));
    serial_print("GDT loaded\n");
    init_idt();
    serial_print("IDT loaded\n");
    kprintf_limine("[Wave Kernel] IDT Has Loaded!\n", sizeof("[Wave Kernel] IDT Has Loaded!\n"));
    pmm_init();
    done();
};
