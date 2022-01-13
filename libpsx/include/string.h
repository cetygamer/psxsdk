/*
 * string.h
 *
 * Prototypes for string functions of the C library
 *
 * PSXSDK
 */

// NOTE: The BIOS was found to be unreliable for many functions,
// so it is not used anymore for the libc.

#ifndef _STRING_H
#define _STRING_H

char *strcat(char *s , const char *append);
char *strncat(char *s , const char *append, int n);
int strcmp(const char *dst , const char *src);
int strncmp(const char *dst , const char *src , int len);
int strcasecmp(const char *s1, const char *s2);
int strncasecmp(const char *s1, const char *s2, int len);
int stricmp(const char *s1, const char *s2); // alias of strcasecmp
int strnicmp(const char *s1, const char *s2, int len); // alias of strncasecmp
char *strcpy(char *dst , const char *src);
char *strncpy(char *dst , const char *src , int n);
int strlen(const char *s);
char *index(const char *s, int c);
char *rindex(const char *s, int c);
char *strchr(const char *s , int c);
char *strrchr(const char *s , int c);
char *strpbrk(const char *dst , const char *src);
int strspn(const char *s , const char *charset);
int strcspn(const char *s , const char *charset);
char *strsep(char **stringp, const char *delim);
char *strtok(char *str, const char *sep);
char *strstr(const char *big , const char *little);
char *strlwr(char *string);
char *strupr(char *string);

void *memset(void *dst , char c , int n);
void *memmove(void *dst , const void *src , int n);
int memcmp(const void *b1 , const void *b2 , int n);
void *memchr(void *s , int c , int n);
void *memcpy(void *dst , const void *src , int n);

#endif

