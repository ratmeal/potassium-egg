// PHOTON INTERNEL "Wave" Kernel
// Code is governed by the GPL-2.0 license.
#include <stdint.h>
#include <stddef.h>
extern void fill_screen(uint64_t color);
extern void draw_rect(uint64_t x, uint64_t y, uint64_t width, uint64_t height, uint64_t color);
void kpanic(const char *message, size_t message_size) {
    // Print the message to the serial port.
    serial_print(message);
    serial_print("\n");
    fill_screen(0xFF0000);
    // draw a white bar across the screen
    // the color white is 0xFFFFFF
    draw_rect(0, 0, 1280, 50, 0xFFFFFF);
    //draw_rect(0, 800, 1280, 50, 0xFFFFFF);
    // Halt the system.
    for (;;) {
        __asm__ volatile ("hlt");
    }
}