#include <psx.h>
#include <stdio.h>
#include "memory.h"

extern int __bss_start[];
extern int __bss_end[];

extern void *__ctor_list;
extern void *__ctor_end;


// Function to call static constructors (for C++, etc.)
static void call_ctors(void)
{
	dprintf("Calling static constructors...\n");

	void **p = &__ctor_end - 1;
	
	for(--p; *p != NULL && (int)*p != -1 && p > &__ctor_list; p--)
	{
		dprintf("Constructor address = %x\n", (unsigned int)*p);
		(*(void (**)())p)();
	}
	
	dprintf("Finished calling static constructors\n");
}

void psxsdk_setup()
{
	unsigned int x;

	printf("Initializing PSXSDK... \n");

	dprintf("Clearing BSS space...\n");
	
// Clear BSS space	
	for(x = (unsigned int)__bss_start; x < (unsigned int)__bss_end; x++)
		*((unsigned char*)x) = 0;

	dprintf("BSS space cleared.\n");
	
// Setup memory allocation functions
	malloc_setup();
	
	dprintf("Finished setting up memory allocation functions.\n");

// Call static constructors
	call_ctors();

}
