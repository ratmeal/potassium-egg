// PHOTON INTERNEL "Wave" Kernel
// Code is governed by the GPL-2.0 license.
#include <stdint.h>
#include <stddef.h>
#include "../Graphics/graphics.h"
void kpanic(const char *message) {
    // Print the message to the serial port.
    serial_print(message);
    serial_print("\n");
    fill_screen(0xFF0000);
    // draw a white bar across the screen
    // the color white is 0xFFFFFF
    draw_rect(0, 0, 1280, 20, 0xFFFFFF);
    put_string(0, 5, "[FATAL] KPANIC!", 0xFF0000);
    put_string(0, 25, "You're computer had a FATAL error and cannot continue", 0xFFFFFF);
    put_string(0, 45, "Please contact your system administrator and give them the following information:", 0xFFFFFF);
    put_string(0, 70, message, 0xFFFFFF);
    //draw_rect(0, 800, 1280, 50, 0xFFFFFF);
    // Halt the system.
    for (;;) {
        __asm__ volatile ("hlt");
    }
}