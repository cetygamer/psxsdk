/*
 * Low-level CDROM library
 */

#include <stdio.h>
#include <stdarg.h>
#include <psx.h>

#define CDREG(x)	*((unsigned char*)(0x1f801800+x))
#define IMASK		*((unsigned int*)0x1f801074)

void _internal_cdrom_handler();
void (*cdrom_handler_callback)();
volatile int cdrom_command_direct = 0;
volatile int cdrom_command_done = 0;
volatile int cdrom_direct_cmd;
volatile int cdrom_command_dpos = 0;
volatile unsigned char cdrom_command_stat[2];

unsigned int cdrom_queue_buf[4] = {0x0, /* Will contain next interrupt handler in queue */
                                    0x0, /* func1 */
				    (unsigned int)_internal_cdrom_handler, /* func2 */
				    0x0, /* pad */
				   };

void CdSendCommand(int cmd, int num, ...)
{
	va_list ap;

	cdrom_command_direct = 1;

	va_start(ap, num);

	CDREG(0) = 0;

	while(num)
	{
		CDREG(2) = (unsigned char)va_arg(ap, unsigned int);
		num--;
	}

	cdrom_command_done = 0;
	CDREG(1) = cdrom_direct_cmd = cmd;
	while(cdrom_command_done == 0);

	cdrom_command_direct = 0;

	va_end(ap);
}

int CdReadResults(unsigned char *out, int max)
{
	unsigned char b;
	unsigned char *outo=out;

	if(max>0)
	{
		*(out++) = cdrom_command_stat[1];
		max--;
	}

	while(CDREG(0) & 0x20)
	{
		b = CDREG(1);
		if(max>0)
		{
			*(out++) = b;
			max--;
		}
	}

	return (out-outo);
}	

void _internal_cdromlib_callback()
{
	// 0 = ACKNOWLEDGE (0x*2)
	// 1 = ACKNOWLEDGE (0x*2), COMPLETE (0x*3)

	unsigned char kind[0x1F] = // 0 = single int, 1 = double int, 2,3,... = others
	{
		0, // Sync 0
		0, // Nop 1
		0, // Setloc 2
		0, // Play 3
		0, // Forward 4
		0, // Backward 5
		0, // ReadN 6
		0, // Standby 7
		0, // Stop 8
		1, // Pause 9
		1, // Init A
		0, // Mute B
		0, // Demute C
		0, // Setfilter D
		0, // Setmode E
		0, // Getmode F
		0, // GetlocL 10
		0, // GetlocP 11
		0xFF, // ??? 12
		0, // GetTN 13
		0, // GetTD 14
		1, // SeekL 15
		1, // SeekP 16
		0xFF, // ??? 17
		0xFF, // ??? 18
		0, // Test 19
		1, // ID 1A
		0, // ReadS 1B
		0, // Reset 1C
		0xFF, // ??? 1D
		1, // ReadToc 1E
	};
	unsigned char i;

	if(cdrom_command_done)
		return;

	CDREG(0) = 1;
	i=CDREG(3);

	if((i&0xf)==5) // Error
		cdrom_command_done = 1;

	switch(kind[cdrom_direct_cmd])
	{
		case 0:
			if(((i&0xf)==3) || ((i&0xf) == 2))
				cdrom_command_done = 1;
		break;

		case 1:
			if((i&0xf)==2)
				cdrom_command_done = 1;
		break;

		case 0xFF: // Unknown command!
				cdrom_command_done = 1;
				return;
		break;
	}

	cdrom_command_stat[0] = i;

	CDREG(0) = 1;
	CDREG(3) = 7;
	i = CDREG(1);
	cdrom_command_stat[1] = i;
}

void _internal_cdromlib_init()
{
	printf("Starting CDROMlib...\n");

	EnterCriticalSection(); // Disable IRQs
	
	SysEnqIntRP(0, cdrom_queue_buf);
	
	IMASK|=4;
	
	cdrom_handler_callback =  _internal_cdromlib_callback;
	
	ExitCriticalSection(); // Enable IRQs
}

int CdGetStatus()
{
	unsigned char out;

	CdSendCommand(CdlNop, 0);
	CdReadResults(&out, 1);

	return out;
}

int CdPlayTrack(unsigned int track)
{
	while(CdGetStatus() & CDSTATUS_SEEK);
	CdSendCommand(CdlSetmode, 1, 0x20);
	CdSendCommand(CdlPlay, 1, ((track/10)<<4)|(track%10));

	return 1;
}
