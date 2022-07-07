#ifndef STRING.h
#define STRING.h
#include <stddef.h>
// [7.24.3] Concatenation functions

char *strcat(char *s, const char *t);
char *strncat(char *s, const char *t, size_t n);

// [7.24.2] Copying functions


char *strcpy(char *s, const char *t);
char *strncpy(char *s, const char *t, size_t n);

// [7.24.4] Comparison functions


int strcmp(char *s, char *t);
int strncmp(const char* s1, const char* s2, size_t n);

// [7.24.5] Search functions

char *strstr(const char *s, const char *t);
char *strtok(char *s, char *delim);

// [7.24.6] Miscellaneous functions


size_t strlen(char *s);
#endif