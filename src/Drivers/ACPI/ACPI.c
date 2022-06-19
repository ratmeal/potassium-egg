#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "../../serial/serial.h"
#include <limine.h>
#include "../../Graphics/graphics.h"
#include "../../klibc/string.h"
#include <lai/core.h>
#include <lai/helpers/sci.h>
#include <lai/helpers/pm.h>
#include <lai/drivers/ec.h>
#include <lai/helpers/pc-bios.h>
#include "../../kpanic/panic.h"
#include "../../memory/vmm.h"
extern struct limine_rsdp_request rsdp_request;
struct acpi_xsdt_t *xsdt;
bool isXSDT = false;

// This function just checks the checksum of an ACPI table
bool doChecksum(struct acpi_header_t *tableHeader)
{
    unsigned char sum = 0;
 
    for (int i = 0; i < tableHeader->length; i++)
    {
        sum += ((char *) tableHeader)[i];
    }
 
    return sum == 0;
}
void PrepareACPI()
{
    // detect if rsdp is acpi 1 or 2
    // to do that we need to check if revision is 0 or 2
    // if it is 0, then we need to use the rsdt address
    // if it is 2, then we need to use the xsdt address

    // get the rsdp address
    uint64_t rsdp_address = rsdp_request.response->address; 
    // get the rsdp
    struct acpi_rsdp_t *rsdpol = (struct acpi_rsdp_t*)rsdp_address;
    // get the revision
    uint8_t revision = rsdpol->revision;
    
    if (revision >= 2)
    {
        struct acpi_xsdp_t *rsdp = (struct acpi_xsdp_t*)rsdp_address;
        isXSDT = true;
        xsdt = (struct XSDT*)rsdp->xsdt;
        serial_print("XSDT detected\n");
        // DEBUG:
        
        lai_enable_tracing(LAI_TRACE_NS);
        lai_set_acpi_revision(xsdt->header.revision);
        
        lai_create_namespace(); // this will kpanic safely 
        serial_print("lai worked....\n");
        
        //lai_enable_acpi(1);
        //lai_create_namespace(); // can't do this until scanning acpi tables is implemented
        // give the pointer to other sdt a size
        // We Do XSDT
        // for (int i = 0; i < ((xsdt->h.Length - sizeof(struct ACPISDTHeader)) / 8); i++)
        // {
        //     // get entry from pointer array as a ACPISDTHeader struct
        //     struct ACPISDTHeader *entry = (struct ACPISDTHeader*)xsdt->pointers[i];
            
        //     // using strncmp to compare the signature
        //     // like this: strncmp(entry->Signature, "APIC", 3);
        //     // it's 3 instead of 4 because there's some weird 4th byte (has to do with it not being null terminated idk)
        //     // in the signature
        //     // Comparring it with 4 bytes gives undefined behavior
        //     if (strncmp(entry->Signature, "FACP", 3) == 0)
        //     {
        //         serial_print("Found FACP\n");
                
        //     }

        // }
    }
    else
    {
        isXSDT = false;
        serial_print("go back to the 90's kid\n");
        serial_print("no your not getting it\n");
        
        // panic here ig
        kpanic("ACPI_REVISION_1_NOT_SUPPORTED");
    }
}

// generic get table function
void *GetTable(char* signature, size_t index)
{
    serial_print(signature);
    serial_print("\n");
    if (signature == "DSDT")
    {
        // we need to get the fadt
        struct acpi_fadt_t *fadt;
        for (int i = 0; i < ((xsdt->header.length - sizeof(struct acpi_header_t)) / 8); i++)
        {
            // get entry from pointer array as a ACPISDTHeader struct
            struct acpi_header_t *entry = (struct acpi_header_t*)xsdt->tables[i];
            // using strncmp to compare the signature
            // like this: strncmp(entry->Signature, "APIC", 3);
            // it's 3 instead of 4 because there's some weird 4th byte (has to do with it not being null terminated idk)
            // in the signature
            // Comparring it with 4 bytes gives undefined behavior
            if (strncmp(entry->signature, "FACP", 3) == 0)
            {
                fadt = (struct acpi_fadt_t*)entry;
                
            }
        }
        return (void*)(fadt->dsdt);
    }
    for (int i = 0; i < ((xsdt->header.length - sizeof(struct acpi_header_t)) / 8); i++)
    {
        
        struct acpi_header_t *entry = (struct acpi_header_t*)xsdt->tables[i];
        if (strncmp(entry->signature, signature, 3) == 0)
        {
            return (void*)(entry);
        }
    }
    return NULL;
}