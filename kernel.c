#include <stdint.h>
#include <stddef.h>
#include <limine.h>
#include <src/gdt.h>
 
// The Limine requests can be placed anywhere, but it is important that
// the compiler does not optimise them away, so, usually, they should
// be made volatile or equivalent.
 
static volatile struct limine_terminal_request terminal_request = {
    .id = LIMINE_TERMINAL_REQUEST,
    .revision = 0
};
struct limine_bootloader_info_request bootloader_info_request = {
    .id = LIMINE_BOOTLOADER_INFO_REQUEST,
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
    terminal_request.response->write(terminal, "PHOTON KERNEL IS \e[0;32mLOADING\e[0m \n", sizeof("PHOTON KERNEL IS \e[0;32mLOADING\e[0m \n"));
    terminal_request.response->write(terminal, "Please Standby... \n", sizeof("Please Standby... \n"));
    struct limine_bootloader_info_response *bootloader_info = bootloader_info_request.response;
    // print out limines version and name in green
    terminal_request.response->write(terminal, "[Kernel] BOOTLOADER: ", sizeof("[Kernel] BOOTLOADER: "));
    terminal_request.response->write(terminal, "\e[0;32m", sizeof("\e[0;32m"));
    terminal_request.response->write(terminal, bootloader_info->name, sizeof(bootloader_info->name));
    terminal_request.response->write(terminal, " V ", sizeof(" V "));
    terminal_request.response->write(terminal, bootloader_info->version, sizeof(bootloader_info->version));
    terminal_request.response->write(terminal, "\e[0m\n", sizeof("\e[0m\n"));
    uint64_t gdt = init_gdt();
    terminal_request.response->write(terminal, "[Kernel] GDT Loaded\n", sizeof("[Kernel] GDT Loaded\n"));
    terminal_request.response->write(terminal, "[Kernel] GDT Entries: ", sizeof("[Kernel] GDT Entries: "));

    //terminal_request.response->write(terminal, "[Kernel] GDT: ", sizeof("[Kernel] GDT: ") - 1);
    // We're done, just hang...
    done();
    
}
