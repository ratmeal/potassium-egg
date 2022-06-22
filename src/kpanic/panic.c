// PHOTON INTERNEL "Wave" Kernel
// Code is governed by the GPL-2.0 license.
#include <stdint.h>
#include <stddef.h>
#include "../Graphics/graphics.h"
#include "../utils.h"
void kpanic(const char *message) {
    // Print the message to the serial port.
    serial_print(message);
    fill_screen(0xFF0000);
    // draw a white bar across the screen
    // the color white is 0xFFFFFF
    draw_rect(0, 0, 1280, 20, 0xFFFFFF);
    put_string(0, 5, "[FATAL] KPANIC!", 0xFF0000, backbuffer, backbuffer_pitch);
    put_string(0, 25, "You're computer had a FATAL error and cannot continue", 0xFFFFFF, backbuffer, backbuffer_pitch);
    put_string(0, 45, "Please contact your system administrator and give them the following information:", 0xFFFFFF, backbuffer, backbuffer_pitch);
    put_string(0, 70, message, 0xFFFFFF, backbuffer, backbuffer_pitch);
    put_string(0, 90, "Register dump:", 0xFFFFFF, backbuffer, backbuffer_pitch);
    put_string(0, 105, "CR3: ", 0xFFFFFF, backbuffer, backbuffer_pitch);
    put_string(35, 105, to_hstring(read_cr3()), 0xFFFFFF, backbuffer, backbuffer_pitch);
    put_string(0, 135, "CR2: ", 0xFFFFFF, backbuffer, backbuffer_pitch);
    put_string(35, 135, to_hstring(read_cr2()), 0xFFFFFF, backbuffer, backbuffer_pitch);
    // Halt the system.
    for (;;) {
        __asm__ volatile ("hlt");
    }
}