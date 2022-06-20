#ifndef GRAPHICS.h
#define GRAPHICS.h
#include <stdbool.h>
extern uint32_t *backbuffer;
extern uint32_t backbuffer_pitch;
void graphics_init();
void draw_pixel(uint64_t x, uint64_t y, uint64_t color);
void fill_screen(uint64_t color);
void draw_rect(uint64_t x, uint64_t y, uint64_t width, uint64_t height, uint64_t color);
void put_string(uint64_t x, uint64_t y, char* str, uint64_t color);
void blit(uint64_t x, uint64_t y, uint64_t width, uint64_t height, uint32_t *src_buffer, uint32_t src_pitch, uint32_t *dst_buffer, uint32_t dst_pitch);
struct GBuffer
{
    uint32_t *buffer;
    uint32_t pitch;
    uint64_t width;
    uint64_t height;
};
#endif