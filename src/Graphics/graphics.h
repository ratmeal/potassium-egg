#ifndef GRAPHICS.h
#define GRAPHICS.h
#include <stdbool.h>

void swap_buffers();
void graphics_init(int screen);
void flush_backbuffer();
void draw_pixel(uint64_t x, uint64_t y, uint64_t color, uint32_t *buffer, uint32_t pitch);
void fill_screen(uint64_t color);
void draw_rect(uint64_t x, uint64_t y, uint64_t width, uint64_t height, uint64_t color);
void put_string(uint64_t x, uint64_t y, char* str, uint64_t color, uint32_t* buffer, uint64_t width);
void blit(uint64_t x, uint64_t y, uint64_t width, uint64_t height, uint32_t *src_buffer, uint64_t src_width, uint32_t *dst_buffer, uint64_t dst_width);
struct GBuffer
{
    uint32_t *buffer;
    uint32_t pitch;
    uint64_t width;
    uint64_t height;
};
extern struct GBuffer Backbuffer;
#endif