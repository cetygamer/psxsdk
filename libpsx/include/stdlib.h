/*
 * stdlib.h
 *
 * Standard library functions
 *
 * PSXSDK
 */

#ifndef _STDLIB_H
#define _STDLIB_H

/* Conversion functions */

int atoi(const char *s);
long atol(const char *s);
char *itoa(int value, char *str, int base);
char *ltoa(long value, char *str, int base);
char *lltoa(long long value, char *str, int base);
char *utoa(unsigned int value, char *str, int base);
char *ultoa(unsigned long value, char *str, int base);
char *ulltoa(unsigned long long value, char *str, int base);
//extern char atob(char *s); // Is this right?


// Random number functions

#define RAND_MAX		0x7fffffff

int rand(void);
void srand(unsigned int seed);

// Quick sort

void qsort(void *base, int nmemb, int size, int (*compar)(const void *, const void *));

// Memory allocation functions

//#warning "malloc() family of functions NEEDS MORE TESTING"

void *malloc(int size);
void free(void *buf);
void *calloc(int number, int size);
void *realloc(void *buf , int n);

int abs(int x);
long long strtoll(const char *nptr, char **endptr, int base);
long strtol(const char *nptr, char **endptr, int base);
double strtod(const char *nptr, char **endptr);
long double strtold(const char *nptr, char **endptr);
float strtof(const char *nptr, char **endptr);

#endif

