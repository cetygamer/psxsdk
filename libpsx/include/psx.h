/*
 * PSXSDK Library include
 */
 
#ifndef _PSX_H
#define _PSX_H

#ifndef true
#define true 1
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef false
#define false 0
#endif

#ifndef FALSE
#define FALSE 0
#endif

/**
 * PSXSDK version information in hexadecimal format.
 *
 * An explanation of version numbers and how they relate to
 * releases:
 * + PSXSDK_VERSION undefined
 *   - PSXSDK 0.1
 * + 0.1.99 (0x0199)
 *   -  From PSXSDK releases 2012-03-03 up to 2013-01-14.
 * + 0.2.99 (0x0299)
 *   - PSXSDK 2013-05-14
 */

#define PSXSDK_VERSION			0x0299

/**
 * PSXSDK version information in string format
 */
 
#define PSXSDK_VERSION_STRING		"0.2.99"



/*
 * Include some GCC builtin includes
 */

#ifndef _PSXSDK_WRAPPER

#include <psxbios.h>

#endif

#include <psxgpu.h>
#include <memcard.h>

#include <psxpad.h>
#include <psxspu.h>
#include <psxcdrom.h>
//#include <adpcm.h>

/**
 * These values below are to be used for evalauting the type field of the
 * psx_pad_state structure
 */

enum psx_pad_types
{
	/** No pad connected. */
	PADTYPE_NONE,
	/** Normal pad. */
	PADTYPE_NORMALPAD,
	/** Analog joystick, the early ones with analog. */
	PADTYPE_ANALOGJOY,
	/** Analog pad, the Dual Shock controller. */
	PADTYPE_ANALOGPAD,
	/** Namco NeGcon. Many steering wheels implement this protocol as well. */
	PADTYPE_NEGCON, // Namco NeGcon
	/** Konami Justifier gun controller. Many third party gun controllers implement this protocol. */
	PADTYPE_KONAMIGUN,
	/** Unknown pad type. */
	PADTYPE_UNKNOWN
};

/**
 * This structure contains the state of the pad (game controller) after
 * polling it with PSX_PollPad()
 */

typedef struct
{
	/** Status. 0 on success, 255 on failure. */
	unsigned char status;
	/** Id. Bits 7-4 indicate the type. Bits 3-0 indicate the number of words
	     in the raw packet returned by the controller. */
	unsigned char id;
	/** Type of pad. To be evaluated with the types in the psx_pad_types enum */
	unsigned char type;
	/** Button bitmask. To be checked by AND'ing with the defines in
	     psxpad.h for buttons. If a bit is set for a button, it is pressed.
	     Checking the pad type to use this bitmask is not necessary at all, 
	     and if button emulations are set up, this may not represent the
	     buttons actually pressed. Also reliable when type is PADTYPE_UNKNOWN */
	unsigned short buttons;
	
	/** Extra data for non-normal controllers. 
	     You should check the value of the type field before accessing any of this data
	*/
	
	union extra
	{
		/** Data for analog joysticks. 
		 *   @attention Due to the poor calibration of the analog sticks, it is recommended
		 *   that you do not assume "left" for X values lower than zero and
		 *   "right" for X values higher than zero.
		 *
		 *   @attention It is recommended to pick a value, for instance 64, and assume
		 *   "left" for values lower than -64, "right" for values higher than 64,
		 *   and no movement for values between -64 and 64.
		 *
		 *   @attention The same is valid for Y values, "up" and "down".
		 **/
		
		struct analogJoy
		{
			/** X coordinates for the left analog stick and the right analog stick 
			 * @par Value 
			 * Totally left: -128, totally right: 127
			*/
			signed char x[2];
			/** Y coordinates for the left analog stick and the right analog stick 
			 * @par Value 
			 * Totally up: -128, totally down: 127
			*/			
			signed char y[2];
		}analogJoy;
		
		/** Data for analog joypads (controller). 
		 *   @attention Due to the poor calibration of the analog sticks, it is recommended
		 *   that you do not assume "left" for X values lower than zero and
		 *   "right" for X values higher than zero.
		 *
		 *   @attention It is recommended to pick a value, for instance 64, and assume
		 *   "left" for values lower than -64, "right" for values higher than 64,
		 *   and no movement for values between -64 and 64.
		 *
		 *   @attention The same is valid for Y values, "up" and "down".
		 **/
		
		struct analogPad
		{
			/** X coordinates for the left analog stick and the right analog stick 
			 * @par Value 
			 * Totally left: -128, totally right: 127
			*/
			signed char x[2];
			/** Y coordinates for the left analog stick and the right analog stick 
			 * @par Value 
			 * Totally up: -128, totally down: 127
			*/			
			signed char y[2];
		}analogPad;
		
		/** Data for Namco NeGcon and steering wheels using its protocol. 
		  * Many steering wheels use this protocol or can switch to it if desired.
		 */
		
		struct negCon
		{
			/**
			 * Steering wheel position. 
			 *
			 * Unlike analog sticks, the steering is accurate and this value is reliable.
			 *
			 *  @par Value
			 * When steering left it is lower than zero, when steering right it is higher than zero
			 * and when not steering at all it is zero.
			 */
			
			signed char steering;
			
			/**
			 * Pressure for button I (1).
			 * @par Value 
			 * 0 = not pressed, 255 = maximum pressure
			 */
			
			unsigned char one;
			
			/**
			 * Pressure for button II (2).
			 * @par Value 
			 * 0 = not pressed, 255 = maximum pressure
			 */
			
			unsigned char two;
			
			/**
			 * Pressure for "L" shoulder button.
			 * @par Value 
			 * 0 = not pressed, 255 = maximum pressure
			 */
			
			unsigned char shoulder;
		}negCon;
	}extra;
}psx_pad_state;

/**
 * Root counter specifications
 */

enum psx_rcnt_specs
{
	/** Pixel clock*/
	RCntCNT0		= 0xf2000000,
	/** Horizontal sync*/
	RCntCNT1		= 0xf2000001,
	/** System clock / 8 */
	RCntCNT2		= 0xf2000002,
	/** VSync (VBlank) */
	RCntCNT3		= 0xf2000003,
};
	
/**
 * Root counter modes
 */

enum psx_rcnt_modes
{
	/** Interrupt mode */
	RCntIntr =	0x1000,
	/** Ignore target and count to 65535 (hex: 0xFFFF) */
	RCntNotar =	0x0100,
	/** Timer stop mode */
	RCntStop =	0x0010,
	 /** System Clock mode */
	RCntSC =		0x0001,
};

struct psx_info
{
	struct kernel
	{
		const char *version; // Kernel version
		int year; // Kernel year
		int month; // Kernel month
		int day; // Kernel day
	}kernel;
	
	struct system
	{
		int memory; // RAM memory size
	}system;
};

/**
 * Initialize library
 */

void PSX_Init();

/**
 * Flags for PSX_Init.
 * PSX_INIT_CD - Initialize CDROM filesystem
 * PSX_INIT_SAVESTATE - Save BIOS state before initializing the library
 */

enum psx_init_flags
{
	PSX_INIT_CD = 1,
	PSX_INIT_SAVESTATE = 2,
};

/**
 * Deinitialize library
 */

void PSX_DeInit();

/**
 * Initialize library (extended version)
 *
 * flags contains a bitmask specifying which flags are enabled
 *
 * PSX_Init() is the same as PSX_InitEx(PSX_INIT_CD)
 * @param flags Flag bitmask (flags are to be OR'd)
 */

void PSX_InitEx(unsigned int flags);

/**
 * Reads the status of the buttons of the two joypads.
 *
 * Takes two pointers for 16-bit bitmasks, one for the first player's joypad,
 * and one for the second one. Their bits are updated to show which buttons were pressed.
 *
 * This function only supplies basic functionality, adequate for a normal digital pad.
 *
 * If more advanced functionality is desired, use PSX_PollPad() instead of this function.
 * @attention Note that some joypads, like the official ones from Sony, do not like to be polled more than 
 * once every 1/60th of a second and if this limitation is not considered the data
 * they return is undefined. Other joypads do not have this limitation but in any case err on the safe side. 
 * @param padbuf Pointer to 16-bit variable where bitmask for pad #1 will be stored.
 *                           If NULL is passed, this argument is ignored.
 * @param padbuf2 Pointer to 16-bit variable where bitmask for pad #2 will be stored
 *                           If NULL is passed, this argument is ignored.
 */
void PSX_ReadPad(unsigned short *padbuf, unsigned short *padbuf2);

/**
 * Polls a joypad for information.
 * @attention Note that some joypads, like the official ones from Sony, do not like to be polled more than 
 * once every 1/60th of a second and if this limitation is not considered the data
 * they return is undefined. Other joypads do not have this limitation but in any case err on the safe side. 
 * @param pad_num Number of the pad to poll (0 = pad #1, 1 = pad #2, etc.)
 * @param pad_state Pointer to a psx_pad_state structure in which to store information for the pad.
 */
 
void PSX_PollPad(int pad_num, psx_pad_state *pad_state);

/**
 * Takes a pointer to a struct psx_info structure, and fills it
 * with information about the PlayStation on which the program is running.
 * PS-OS kernel build date and version are reported among other things.
 * @param info Pointer to struct psx_info structure which will be filled.
 */
void PSX_GetSysInfo(struct psx_info *info);

/**
 * Gets Coprocessor 0 status register
 * @return Value of Coprocessor 0 status register
 */
 
unsigned int get_cop0_status();

/**
 * Sets Coprocessor 0 status register
 * @param sreg New value of Coprocessor 0 status register
 * @return [[ check: maybe this is a void function ]]
 */

unsigned int set_cop0_status(unsigned int sreg);

/**
 * Gets the contents of the program counter when the 
 * last exception happened.
 * @return Value of the program counter at the time of the last exception
 */
 
unsigned int get_cop0_epc();

/**
 * Get value of specified Coprocessor 0 register
 * @param register_num Number of Coprocessor 0 register whose value must be retrieved
 * @return Value of specified Coprocessor 0 register
 */

unsigned int get_cop0_register(unsigned char register_num);

/**
 * Set value of specified Coprocessor 0 register
 * @param register_num Number of Coprocessor 0 register whose value must be set
 * @param value New value of specified Coprocessor 0 register
 */

void set_cop0_register(unsigned char register_num, unsigned int value);

// Root counter functions

/**
 * Set root counter (documentation TO DO)
 * @param spec Spec
 * @param target Target
 * @param mode mode
 */

int SetRCnt(int spec, unsigned short target, unsigned int mode);

/**
 * Get root counter (documentation TO DO)
 * @param spec Spec
 * @return TO DO
 */

int GetRCnt(int spec);

/**
 * Start root counter (documentation TO DO)
 * @param spec Spec
 * @return TO DO
 */

int StartRCnt(int spec);

/**
 * Stop root counter (documentation TO DO)
 * @param spec Spec
 * @return TO DO
 */

int StopRCnt(int spec);

/**
 * Restores BIOS state to the one prior the initialization of the library.
 * This function can only be used if PSX_InitEx() was called with the
 * PSX_INIT_SAVESTATE flag.
 * It is usually called by PSX_DeInit() automatically, so unless
 * you know what you are doing, you do not need to call it yourself.
 * @return 1 on success, 0 on failure, such as the fact that PSX_InitEx() wasn't called
 * with the PSX_INIT_SAVESTATE flag.
 */

int PSX_RestoreBiosState();

/**
 * Gets the bitmask for the flags passed to PSX_InitEx()
 * @return Flag bitmask
 */
 
unsigned int PSX_GetInitFlags();

#endif
