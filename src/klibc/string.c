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
int strncmp(const char* s1, const char* s2, size_t n) {
    while (n && *s1 && (*s1 == *s2)){
        ++s1;
        ++s2;
        --n;
    }

    if (n == 0) {
        return 0;
    } else {
        return (*(unsigned char *)s1 - *(unsigned char *)s2);
    }
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
bool is_delim(char c, char *delim)
{
  while(*delim != '\0')
  {
    if(c == *delim)
      return true;
    delim++;
  }
  return false;
}
char *strtok(char *s, char *delim)
{
  static char *p; // start of the next search 
  if(!s)
  {
    s = p;
  }
  if(!s)
  {
    // user is bad user 
    return NULL;
  }

  // handle beginning of the string containing delims
  while(1)
  {
    if(is_delim(*s, delim))
    {
      s++;
      continue;
    }
    if(*s == '\0')
    {
      return NULL; // we've reached the end of the string
    }
    // now, we've hit a regular character. Let's exit the
    // loop, and we'd need to give the caller a string
    // that starts here.
    break;
  }

  char *ret = s;
  while(1)
  {
    if(*s == '\0')
    {
      p = s; // next exec will return NULL
      return ret;
    }
    if(is_delim(*s, delim))
    {
      *s = '\0';
      p = s + 1;
      return ret;
    }
    s++;
  }
}