#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "../serial/serial.h"
#include "../../limine.h"
#include "../utils.h"
#include "fonts/linux_font_8x16.h"
#include "../memory/heap.h"
extern struct limine_framebuffer_request framebuffer_request;
uint32_t *backbuffer;
uint16_t backbuffer_pitch;


void draw_pixel(uint64_t x, uint64_t y, uint64_t color, uint32_t *buffer, uint32_t pitch)
{
    if (x >= pitch || y >= pitch)
        return;
    buffer[y * pitch + x] = color;
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
            backbuffer[y * (backbuffer_pitch) + x] = color;
            
        }
    }
    swap_buffers();
}

void draw_rect(uint64_t x, uint64_t y, uint64_t width, uint64_t height, uint64_t color)
{
    // draw a rectangle
    for (uint64_t i = 0; i < width; i++)
    {
        for (uint64_t j = 0; j < height; j++)
        {
            backbuffer[(y + j) * (backbuffer_pitch) + (x + i)] = color;
        }
    }
    swap_buffers();
}
void graphics_init()
{
    backbuffer_pitch = framebuffer_request.response->framebuffers[0]->pitch;
    backbuffer = (uint32_t*)malloc(framebuffer_request.response->framebuffers[0]->height * backbuffer_pitch);
    //serial_print("no page fault\n");
    flush_backbuffer();
}
void flush_backbuffer()
{
    memset(backbuffer, 0, framebuffer_request.response->framebuffers[0]->width * backbuffer_pitch);
}
void swap_buffers()
{
    memcpy32((uint32_t*)framebuffer_request.response->framebuffers[0]->address, backbuffer, framebuffer_request.response->framebuffers[0]->height * backbuffer_pitch);

}

void blit(uint64_t x, uint64_t y, uint64_t width, uint64_t height, uint32_t *src_buffer, uint32_t src_pitch, uint32_t *dst_buffer, uint32_t dst_pitch)
{
    for (uint64_t i = 0; i < width; i++)
    {
        for (uint64_t j = 0; j < height; j++)
        {
            dst_buffer[(j + y) * dst_pitch + (i + x)] = src_buffer[j * src_pitch + i];
        }
    }
}
void draw_char(uint64_t x, uint64_t y, char c, uint64_t color)
{
    unsigned char* font = (unsigned char*)fontdata_8x16.data;
    for (uint64_t i = 0; i < 16; i++)
    {
        for (uint64_t j = 0; j < 8; j++)
        {
            if ((font[(c * 16) + i] & (0x80 >> j)) != 0)
            {
                draw_pixel(x + j, y + i, color, backbuffer, backbuffer_pitch);
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
        if (str[i] == '\n')
        {
            continue;   
        }
        draw_char(x + (i * 8), y, str[i], color);
    }
    swap_buffers();
}
void draw_image_raw(uint64_t x, uint64_t y, uint64_t width, uint64_t height, uint32_t* data, uint32_t *buffer, uint32_t pitch)
{
    for (uint64_t i = 0; i < width; i++)
    {
        for (uint64_t j = 0; j < height; j++)
        {
            draw_pixel(x + i, y + j, data[j * width + i], buffer, pitch);
        }
    }
}