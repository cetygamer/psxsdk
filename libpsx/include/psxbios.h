/*
 * PSXSDK: Bios functions
 */

#ifndef _PSXBIOS_H
#define _PSXBIOS_H

/* Joypad functions */

extern void PAD_init(unsigned long mode, unsigned long *pad_buf);
extern int PAD_dr();

/* ROM information functions */

/** 
 * Returns PSX kernel date.
 * @return Kernel date n 0xYYYYMMDD BCD format.
 */
 
unsigned long GetKernelDate();

/**
 * Returns a pointer to a zero-terminated
 * string which contains the kernel ROM version.
 * @return Pointer to a zero-terminated string which contains the kernel ROM version.
 */

const char *GetKernelRomVersion();

/**
 * Returns a pointer to a zero-terminated
 * string which contains the system ROM version.
 * @return Zero-terminated string which contains the system ROM version.
 */

const char *GetSystemRomVersion();

/**
 * GetRamSize() should return size of RAM in bytes.
 * It doesn't seem to work most times. On SCPH1001, it returns 0.
 * On SCPH1000, it returns 2 (which is the number of megabytes of RAM
 * the PSX has.)
 * @return Size of RAM in bytes.
 */

unsigned int GetRamSize();

/* Interrupt/Exception functions */

/*void Exception();*/

/**
 * Enters a critical section.
 */

void EnterCriticalSection();

/**
 * Exits a critical section.
 */

void ExitCriticalSection();

void SysEnqIntRP(int index, unsigned int *buf);
void SysDeqIntRP(int index, unsigned int *buf);

void ResetEntryInt();



struct DIRENTRY
{
	char name[20]; // Filename
	int attr; // Attributes
	int size; // File size in bytes
	struct DIRENTRY *next; // Pointer to next file entry
	char system[8]; // System reserved
};

/**
 * Gets information about the first file which
 * matches the pattern. ? and * wildcards can be used.
 * Characters after * are ignored.
 * @param name File name string
 * @param dirent Pointer to a struct DIRENTRY object.
 * @return Pointer to a struct DIRENTRY object. 
 */

struct DIRENTRY *firstfile(char *name, struct DIRENTRY *dirent);

/** 
 * Gets the file size of the file named "name".
 * It is actually just a wrapper around firstfile.
 * It rounds the file size to the block size (2048).
 * @param name FIle name string
 * @return File size in bytes, rounded.
 */

int get_file_size(char *name);

/**
 * This function is like get_file_size() but doesn't round
 * the file size to the block size.
 * @param name File name string
 * @return File size in bytes, unrounded.
 */
 
int get_real_file_size(char *name);

void InitHeap(void *block , int size);
void FlushCache();

void SetVBlankHandler(void (*h)());
void RemoveVBlankHandler();

void SetRCntHandler(void (*callback)(), int spec, unsigned short target);

/**
 * Opens an event, and returns its identifier
 * Must be executed in a critical section
 * @param desc Numerical cause descriptor
 * @param type Numerical event type
 * @param mode Numerical mode
 * @param func Function pointer to callback function
 * @return Numerical identifier for the event opened
 */

int OpenEvent(
	int desc, // Cause descriptor
	int spec, // Event type
	int mode, // Mode
	int *(*func)() // Pointer to callback function
);

/**
 * Enables an event by its identifier returned by OpenEvent()
 * @param Numerical event identifier
 * @return ???
 */

int EnableEvent(unsigned int event);

/**
 * Closes an event by its identifier
 * @param Numerical event identifier
 * @return ???
 */

int CloseEvent(unsigned int event);

/**
 * Disables an event by its identifier
 * @param Numerical event identifier
 * @return ???
 */

int DisableEvent(unsigned int event);

/**
 * Generates an event. This must be executed in a critical section.
 * If the event to deliver is set to generate an interrupt, the handler function is called.
 * @param Numerical cause descriptor
 * @param Numerical event class
 * @return ???
 */

int DeliverEvent(unsigned int ev1, // Cause descriptor
			  int ev2); // Event class
			  
/**
 * Checks if the event specified by its identifier has occured
 * @param event Numerical event identifier
 * @return 1 if the event has occured, 0 if it has not
 */

int TestEvent(unsigned int event);

/**
 * Waits until the event specified by identifier occurs.
 * @param event Numerical event identifier
 * @return 1 on success, 0 on failure.
 */

int WaitEvent(unsigned int event);

/**
 * Replaces the executable image in memory with the one
 * contained in another executable file in PSX-EXE format.
 * WARNING: Does not work right now.
 * @param name Path name of PSX-EXE executable
 * @param argc Number of arguments
 * @param argv Pointer to an array of string pointers for each argument
 */
 
void LoadExec(char *name, int argc, char **argv);

#endif
