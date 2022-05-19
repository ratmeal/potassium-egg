#include <stdint.h>
#include <limine.h>
// dec to string
void dec_to_str(uint64_t dec, char* str) 
{
    str[sizeof(str) - 1] = '\0';
    for (int i = 18; i >= 0; i--) {
       str[i] = dec % 10 + 48;
       dec /= 10;
    }
};
// hex to string
void hex_to_str(uint64_t hex, char* str) 
{
    str[sizeof(str) - 1] = '\0';
    for (int i = 15; i >= 0; i--) {
       str[i] = hex % 16 + 48;
       hex /= 16;
    }
};
struct limine_terminal_request terminal_request;
struct limine_terminal* terminal;
void kprintf_limine(char* str, unsigned long length) 
{
    terminal_request.response->write(terminal, str, length);
};
// set limine terminal without optimizing
void set_limine_terminal(struct limine_terminal_request volatile request, struct limine_terminal* volatile terminal) 
{
    terminal_request = request;
    terminal = terminal;
};