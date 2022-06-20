#ifndef WINDOW.h
#define WINDOW.h
struct Window
{
    struct GBuffer fb;
    char *title;
    uint64_t pos_x;
    uint64_t pos_y;
};
struct Window *init_window(char* title, uint64_t pos_x, uint64_t pos_y, uint64_t width, uint64_t height);
void draw_window(struct Window *window);
#endif