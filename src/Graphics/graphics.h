#ifndef GRAPHICS.h
#define GRAPHICS.h
void draw_pixel(uint64_t x, uint64_t y, uint64_t color);
void fill_screen(uint64_t color, bool overwrite);
void clear_screen(uint64_t color);
void draw_rect(uint64_t x, uint64_t y, uint64_t width, uint64_t height, uint64_t color);
void put_string(uint64_t x, uint64_t y, char* str, uint64_t color);
#endif