#include <stddef.h>
#include <stdint.h>
#include <limine.h>
#include <stdbool.h>
#include "../../../../utils.h"
#include "../../../../serial/serial.h"
#include "../../../../Graphics/graphics.h"
#include "../../../../memory/heap.h"
#include "Window.h"
struct Window *init_window(char* title, uint64_t pos_x, uint64_t pos_y, uint64_t width, uint64_t height)
{
    struct Window *window = malloc(sizeof(struct Window));
    window->pos_x = pos_x;
    window->pos_y = pos_y;
    window->fb.width = width;
    window->fb.height = height;
    window->title = title;
    window->fb.pitch = (uint32_t)width * 4;
    window->fb.buffer = malloc(window->fb.height * window->fb.pitch);
    return window;
}
// this code is so bad that it's worth it to just end it all because it's so bad
void draw_window(struct Window *window)
{
    for (uint64_t i = 0; i < window->fb.height; i++)
    {
        for (uint64_t j = 0; j < window->fb.width; j++)
        {
            window->fb.buffer[i * window->fb.width + j] = 0x80808080;
        }
    }
    blit(window->pos_x, window->pos_y, window->fb.width, window->fb.height, window->fb.buffer, window->fb.width, Backbuffer.buffer, Backbuffer.pitch / 4);
    put_string(window->pos_x, window->pos_y, window->title, 0xEEEEEE, Backbuffer.buffer, Backbuffer.pitch / 4);
    swap_buffers();
}