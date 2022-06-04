#include "../utils.h"
#include <stddef.h>
int strcmp(char *s, char *t)
{
    for ( ; *s == *t; s++, t++)
    if (*s == '\0' )
    return 0;
    return *s - *t;
}
// strncmp
int strncmp(char *s, char *t, size_t n)
{
    for ( ; n > 0 && *s == *t; s++, t++, n--)
    if (n == 0 || *s == '\0')
    return 0;
    return *s - *t;
}
size_t strlen(char *s)
{
    int i;
    i = 0;
    while (s[i] != '\0')
        i++;
    return i;
}

char *strstr(const char *s, const char *t)
{
    size_t n = strlen(t);
    for ( ; *s; s++)
    if (strncmp(s, t, n) == 0)
    return (char *)s;
    return NULL;
}

// strcpy
char *strcpy(char *s, const char *t)
{
    char *os = s;
    while ((*s++ = *t++) != '\0')
    continue;
    return os;
}
// strncpy
char *strncpy(char *s, const char *t, size_t n)
{
    char *os = s;
    while (n-- && (*s++ = *t++) != '\0')
    continue;
    while (n--)
    *s++ = '\0';
    return os;
}
// strcat
char *strcat(char *s, const char *t)
{
    char *os = s;
    while (*s != '\0')
    s++;
    while ((*s++ = *t++) != '\0')
    continue;
    return os;
}

char *strncat(char *s, const char *t, size_t n)
{
    char *os = s;
    while (*s != '\0')
    s++;
    while (n-- && (*s++ = *t++) != '\0')
    continue;
    return os;
}