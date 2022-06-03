#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "../serial/serial.h"
#include "../../limine.h"
#include "../utils.h"
#include "fonts/linux_font_8x16.h"
extern struct limine_framebuffer_request framebuffer_request;
// uint32_t *backbuffer;



// // draw pixel
void draw_pixel(uint64_t x, uint64_t y, uint64_t color)
{
    // when you have double buffering, you can use the backbuffer
    // backbuffer[y * (framebuffer_request.response->framebuffers[0]->pitch / sizeof(uint32_t)) + x] = color;
    
    *((uint32_t*)(framebuffer_request.response->framebuffers[0]->address + 4 * (framebuffer_request.response->framebuffers[0]->pitch / 4) * y + 4 * x)) = color;
}

void fill_screen(uint64_t color, bool overwrite)
{
    // get the height and width of the framebuffer
    uint64_t width = framebuffer_request.response->framebuffers[0]->width;
    uint64_t height = framebuffer_request.response->framebuffers[0]->height;
    
    // draw a big rectangle covering the whole screen
    for (uint64_t x = 0; x < width; x++)
    {
        for (uint64_t y = 0; y < height; y++)
        {
            if (overwrite)
            {
                draw_pixel(x, y, color);
            }
            else
            {
                // check if there is a pixel already there
                if (*((uint32_t*)(framebuffer_request.response->framebuffers[0]->address + 4 * (framebuffer_request.response->framebuffers[0]->pitch / 4) * y + 4 * x)) == 0)
                {
                    draw_pixel(x, y, color);
                }
            }
            
        }
    }
    // swap_buffers();
}
void clear_screen(uint64_t color)
{
    uint64_t width = framebuffer_request.response->framebuffers[0]->width;
    uint64_t height = framebuffer_request.response->framebuffers[0]->height;
    for (uint64_t x = 0; x < width; x++)
    {
        for (uint64_t y = 0; y < height; y++)
        {
            if (*((uint32_t*)(framebuffer_request.response->framebuffers[0]->address + 4 * (framebuffer_request.response->framebuffers[0]->pitch / 4) * y + 4 * x)) == color)
            {
                draw_pixel(x, y, 0);
            }
        }
    }
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
void draw_char(uint64_t x, uint64_t y, char c, uint64_t color)
{
    unsigned char* font = (unsigned char*)fontdata_8x16.data;
    for (uint64_t i = 0; i < 16; i++)
    {
        for (uint64_t j = 0; j < 8; j++)
        {
            if ((font[(c * 16) + i] & (0x80 >> j)) != 0)
            {
                draw_pixel(x + j, y + i, color);
            }
        }
    }
}


void put_string(uint64_t x, uint64_t y, char* str, uint64_t color)
{
    int len;
    for (len = 0; str[len] != '\0'; len++);
    for (uint64_t i = 0; i < len; i++)
    {
        draw_char(x + (i * 8), y, str[i], color);
    }
}