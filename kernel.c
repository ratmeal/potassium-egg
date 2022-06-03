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
#include "src/Graphics/graphics.h"
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
static uint64_t last_frame_color = 0x000000;
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
    put_string(0, 0, "[KERNEL] GDT Loaded!", 0xFFFFFF);
    put_string(0, 16, "[KERNEL] IDT Loaded!", 0xFFFFFF);
    serial_print("PMM loaded\n");
    put_string(0, 32, "[KERNEL] PMM Loaded!", 0xFFFFFF);
    put_string(0, 48, "[KERNEL] Initializing the VMM...", 0xFFFFFF);
    // free memory in kilobytes
    //kpanic("don don", sizeof("don don"));
    //graphics_init();
    //fill_screen(0x018281);
    // read serial
    vmm_init();
    serial_print("VMM loaded\n");
    put_string(0, 96, "[KERNEL] VMM Loaded!", 0xFFFFFF);
    put_string(0, 112, "[KERNEL] Starting Fallback Serial Command Line...", 0xFFFFFF);
    command_line_system();
    done();
};

void command_line_system()
{
    while(1)
    {
    serial_print("\n");
    serial_print("> ");
    char c = read_serial();
    switch (c)
    {
        case 'v':
            serial_print("\nPhoton ");
            serial_print("0.0.1");
            serial_print("\nWave Kernel ");
            serial_print("0.0.1");
            serial_print("\nCopyright (C) 2022 The Photon Team");
            break;
        case 'h':
            serial_print("\nUsage:\n");
            serial_print("v - Prints Version Information\n");
            serial_print("h - Prints this help message");
            break;
        case 's':
            serial_print("What do you want to do?\n");
            serial_print(": ");
            char d = read_serial();
            switch (d)
            {
                case 'f':
                    serial_print("\nWhat do you want to fill the screen with?\n");
                    serial_print("Options:\n");
                    serial_print("0 - Black\n");
                    serial_print("1 - Sussy\n");
                    serial_print("2 - Red\n");
                    serial_print("3 - Windows 98 BG Color\n");
                    serial_print(": ");
                    char e = read_serial();
                    switch (e)
                    {
                        case '0':
                            clear_screen(last_frame_color);
                            fill_screen(0x000000, false);
                            last_frame_color = 0x000000;
                            break;
                        case '1':
                            clear_screen(last_frame_color);
                            fill_screen(0xF8FFFF, false);
                            last_frame_color = 0xF8FFFF;
                            break;
                        case '2':
                            clear_screen(last_frame_color);
                            fill_screen(0xFF0000, false);
                            last_frame_color = 0xFF0000;
                            break;
                        case '3':
                            clear_screen(last_frame_color);
                            fill_screen(0x008080, false);
                            last_frame_color = 0x008080;
                            break;
                        default:
                            serial_print("\nInvalid option\n");
                            break;
                    }
                    break;
                default:
                    serial_print("\nInvalid option\n");
                    break;
            }
            break;
    }

    }
}