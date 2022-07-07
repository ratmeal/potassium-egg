#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <limine.h>
#include "../../serial/serial.h"
#include "../ACPI/ACPI.h"
#include <lai/core.h>
#include <lai/helpers/sci.h>
#include <lai/helpers/pm.h>
#include <lai/drivers/ec.h>
#include <lai/helpers/pc-bios.h>
extern struct limine_hhdm_request hhdm_request;
struct hpet_table {
	struct acpi_header_t acpi_hdr;
	uint8_t hardware_rev_id;
	uint8_t info;
	uint16_t pci_id;
	uint8_t address_space_id;
	uint8_t register_width;
	uint8_t register_offset;
	uint8_t reserved;
	uint64_t address;
	uint8_t hpet_num;
	uint16_t minim_ticks;
	uint8_t page_protection;
} __attribute__((packed));

struct hpet_regs {
	uint64_t capabilities;
	uint64_t unused0;
	uint64_t general_config;
	uint64_t unused1;
	uint64_t int_status;
	uint64_t unused2;
	uint64_t unused3[24];
	uint64_t counter_value;
	uint64_t unused4;
} __attribute__((packed));
static volatile struct hpet_table *hpet_table;
static volatile struct hpet_regs *hpet_regs;
void hpet_init() {
	hpet_table = GetTable("HPET", 0);
	hpet_regs = (struct hpet_regs*)(hpet_table->address + hhdm_request.response->offset);
	hpet_regs->general_config = 1; 
    serial_print("Done!\n");
} 
void sleep(size_t ms) {
	size_t ticks = (hpet_regs->counter_value + (ms * 1000000000000)) / ((hpet_regs->capabilities >> 32) & 0xffffffff);
	for(;hpet_regs->counter_value < ticks;); 
}