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
    window->fb.pitch = width / sizeof(uint32_t);
    window->fb.buffer = malloc(window->fb.height * window->fb.pitch);
    return window;
}
void draw_window(struct Window *window)
{
    // we need to draw a border and a decoration for the window
    // we do this onto the backbuffer
    serial_print("preparring\n");
    blit(window->pos_x, window->pos_y, window->fb.width, window->fb.height, window->fb.buffer, window->fb.pitch, backbuffer, backbuffer_pitch);
    // we need to draw the title
    // we do this onto the backbuffer
    put_string(window->pos_x + 2, window->pos_y + 2, window->title, 0xFFFFFFFF);
}