/*
 * stdio.h implementation for PSXSDK
 */

#ifndef _STDIO_H
#define _STDIO_H

#ifdef _PSXSDK_WRAPPER

/*
 * Dirty hack... 
 */

#include "/usr/include/stdio.h"

#else

#include <types.h>

#include <stdarg.h>
#include <stdbool.h>

#define SEEK_SET	0
#define SEEK_CUR	1
#define SEEK_END	2

#define EOF		-1

/* NULL */
#ifndef NULL
#define NULL (void*)0
#endif

enum stdio_directions
{
	STDIO_DIRECTION_BIOS,
	STDIO_DIRECTION_SIO
};

enum file_devices
{
	FDEV_UNKNOWN,
	FDEV_CDROM,
	FDEV_MEMCARD,
	FDEV_CONSOLE
};

extern int __stdio_direction;

/**
 * File stream
 */

typedef struct
{
	 /** File descriptor, as returned by open() */
	int fildes;
	 /** Current file position */
	unsigned int pos;
	/** File access mode */
	unsigned int mode; 
	 /** Device ID */
	unsigned int dev;
	 /** Size in bytes */
	unsigned int size;
	 /** Used internally by fopen(), 0 if free, 1 if occupied */
	unsigned int used;
	/** End-of-File marker */
	unsigned int eof;
	/** Error marker */
	unsigned int error;
}FILE;

/* Console functions */

int putchar(int c);
int puts(const char *str);

/**
 * BIOS printf() implementation. Does not support floating point.
 * NOTE: when redirect_stdio_to_sio() is used, PSXSDK's internal implementation is used instead.
 */

extern int printf(const char *format, ...);


#ifdef __IN_LIBPSX

// Only for code in libpsx

// If PSXSDK_DEBUG is defined, dprintf() calls are turned into printf() calls
// otherwise they are left out

#ifdef PSXSDK_DEBUG
	#define dprintf		printf
#else
	#define dprintf(fmt, ...)
#endif

#endif

int vsnprintf(char *string, size_t size, const char *fmt, va_list ap);
int vsprintf(char *string, const char *fmt, va_list ap);
int sprintf(char *string, const char *fmt, ...);
int snprintf(char *string, size_t size, const char *fmt, ...);
int vprintf(char *fmt, va_list ap);

FILE *fdopen(int fildes, const char *mode);
FILE *fopen(const char *path, const char *mode);
int fclose(FILE *stream);
int fread(void *ptr, int size, int nmemb, FILE *f);
int fwrite(void *ptr, int size, int nmemb, FILE *f);

int fgetc(FILE *f);
int ftell(FILE *f);
int fseek(FILE *f, int offset, int whence);

int fputs(const char *str, FILE *stream);
void clearerr(FILE *stream);
int feof(FILE *stream);
int ferror(FILE *stream);
int fileno(FILE *stream);

#define getc(f)		fgetc(f)

int rename(const char *oldname, const char *newname);
int remove(const char *filename);

#ifndef __cplusplus
// Define delete(x) to be remove(x) only when compiling plain C.
#define delete(x)	remove(x)
#endif

/**
 * Redirects STDIO to SIO (serial port)
 */
 
void redirect_stdio_to_sio(void);

/**
 * Sets whether a carriage return must be written before a line feed.
 * In simpler words, whether '\n' must be translated to a '\r\n' sequence.
 * If you come from the Unix world, you most likely want to set this.
 *
 * @param setting New status of the setting (0 = disabled, 1 = enabled)
 */
 
void sio_stdio_mapcr(unsigned int setting);

/**
 * scanf and friends
 */
 
int vsscanf(const char *str, const char *fmt, va_list ap);
int sscanf(const char *str, const char *fmt, ...);


/**
 * STDIO for SIO 
 */
 
int sio_putchar(int c);
int sio_puts(const char *str);
int sio_printf(const char *fmt, ...);
int sio_vprintf(const char *fmt, va_list ap);

#endif

#endif

