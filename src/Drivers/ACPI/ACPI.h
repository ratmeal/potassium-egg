#ifndef ACPI.h
#define ACPI.h
#include <stddef.h>
void *GetTable(char* signature, size_t index);
#endif