#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "../serial/serial.h"
#include "../../limine.h"
#include "../utils.h"
extern struct limine_framebuffer_request framebuffer_request;
// uint32_t *backbuffer;



// // draw pixel
void draw_pixel(uint64_t x, uint64_t y, uint64_t color)
{
    // when you have double buffering, you can use the backbuffer
    // backbuffer[y * (framebuffer_request.response->framebuffers[0]->pitch / sizeof(uint32_t)) + x] = color;
    
    *((uint32_t*)(framebuffer_request.response->framebuffers[0]->address + 4 * (framebuffer_request.response->framebuffers[0]->pitch / 4) * y + 4 * x)) = color;
}

void fill_screen(uint64_t color)
{
    // get the height and width of the framebuffer
    uint64_t width = framebuffer_request.response->framebuffers[0]->width;
    uint64_t height = framebuffer_request.response->framebuffers[0]->height;
    
    // draw a big rectangle covering the whole screen
    for (uint64_t x = 0; x < width; x++)
    {
        for (uint64_t y = 0; y < height; y++)
        {
            draw_pixel(x, y, color);
        }
    }
    // swap_buffers();
}

void draw_rect(uint64_t x, uint64_t y, uint64_t width, uint64_t height, uint64_t color)
{
    // draw a rectangle
    for (uint64_t i = 0; i < width; i++)
    {
        for (uint64_t j = 0; j < height; j++)
        {
            draw_pixel(x + i, y + j, color);
        }
    }
}
// void graphics_init()
// {
    
//     backbuffer = (uint32_t*)kmalloc(framebuffer_request.response->framebuffers[0]->width * framebuffer_request.response->framebuffers[0]->height * (framebuffer_request.response->framebuffers[0]->bpp / 8));
//     serial_print("no page fault\n");
//     flush_backbuffer();
// }
// void flush_backbuffer()
// {
//     serial_print("still no page fault\n");
//     memset(backbuffer, 0, framebuffer_request.response->framebuffers[0]->width * framebuffer_request.response->framebuffers[0]->pitch);
// }
// void swap_buffers()
// {
//     memcpy32((uint32_t*)framebuffer_request.response->framebuffers[0]->address, backbuffer, framebuffer_request.response->framebuffers[0]->height * framebuffer_request.response->framebuffers[0]->pitch);
// }