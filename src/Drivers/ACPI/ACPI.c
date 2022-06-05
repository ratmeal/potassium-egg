#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "../../serial/serial.h"
#include "../../utils.h"
#include <limine.h>
#include "../../Graphics/graphics.h"
#include "../../klibc/string.h"
extern struct limine_rsdp_request rsdp_request;
struct ACPISDTHeader {
  char Signature[4];
  uint32_t Length;
  uint8_t Revision;
  uint8_t Checksum;
  char OEMID[6];
  char OEMTableID[8];
  uint32_t OEMRevision;
  uint32_t CreatorID;
  uint32_t CreatorRevision;
} __attribute__((packed));
// ACPI 1
struct RSDPDescriptor {
 char Signature[8];
 uint8_t Checksum;
 char OEMID[6];
 uint8_t Revision;
 uint32_t RsdtAddress;
} __attribute__ ((packed));
// ACPI 2
struct RSDPDescriptor20 {
 struct RSDPDescriptor firstPart;
 
 uint32_t Length;
 uint64_t XsdtAddress;
 uint8_t ExtendedChecksum;
 uint8_t reserved[3];
} __attribute__ ((packed));
// XSDT struct with SDT Header and pointers to other SDT
struct XSDT {
    struct ACPISDTHeader h;
    uint64_t pointers[];
} __attribute__ ((packed));
// This function just checks the checksum of an ACPI table
bool doChecksum(struct ACPISDTHeader *tableHeader)
{
    unsigned char sum = 0;
 
    for (int i = 0; i < tableHeader->Length; i++)
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
    struct RSDPDescriptor20 *rsdp = (struct RSDPDescriptor20*)rsdp_address;
    // get the revision
    uint8_t revision = rsdp->firstPart.Revision;
    if (revision >= 2)
    {
        struct XSDT *xsdt = (struct XSDT*)rsdp->XsdtAddress;
        // give the pointer to other sdt a size
        
        // We Do XSDT
        for (int i = 0; i < ((xsdt->h.Length - sizeof(struct ACPISDTHeader)) / 8); i++)
        {
            // get entry from pointer array as a ACPISDTHeader struct
            struct ACPISDTHeader *entry = (struct ACPISDTHeader*)xsdt->pointers[i];
            
            // using strncmp to compare the signature
            // like this: strncmp(entry->Signature, "APIC", 3);
            // it's 3 instead of 4 because there's some weird 4th byte (has to do with it not being null terminated idk)
            // in the signature
            // Comparring it with 4 bytes gives undefined behavior
            if (strncmp(entry->Signature, "FACP", 3) == 0)
            {
                serial_print("Found FACP\n");
                
            }

        }
    }
    else
    {
        serial_print("go back to the 90's kid\n");
    }
}
