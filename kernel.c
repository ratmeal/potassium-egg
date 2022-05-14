#include <stdint.h>
#include <stddef.h>
#include <limine.h>
 
// The Limine requests can be placed anywhere, but it is important that
// the compiler does not optimise them away, so, usually, they should
// be made volatile or equivalent.
 
static volatile struct limine_terminal_request terminal_request = {
    .id = LIMINE_TERMINAL_REQUEST,
    .revision = 0
};

static void done(void) {
    for (;;) {
        __asm__("hlt");
    }
}

// The following will be our kernel's entry point.
void _start(void) {
    // Ensure we got a terminal
    if (terminal_request.response == NULL
     || terminal_request.response->terminal_count < 1) {
        done();
    }
 
    // We should now be able to call the Limine terminal to print out
    // a simple "Hello World" to screen.
    struct limine_terminal *terminal = terminal_request.response->terminals[0];
    terminal_request.response->write(terminal, "PHOTON KERNEL IS \e[0;32mLOADING\e[0m \n", sizeof("PHOTON KERNEL IS \e[0;32mLOADING\e[0m \n") - 1);
    terminal_request.response->write(terminal, "Please Standby... \n", sizeof("Please Standby... \n") - 1);
    
 
    // We're done, just hang...
    done();
}