#include <psx.h>

unsigned int get_cop0_register(unsigned char register_num)
{
// Workaround for MIPS' simplicistic instruction set...	
	
	unsigned int instr[] =
		{0x40020000, // mfc $v0, 0
		  0x03E00008, // jr $ra
		  0x00000000, // nop
                  0x00000000}; // nop

	int (*rawFunc)() = (void*)instr;

// Write coprocessor register number inside instruction		  
	instr[0] |= (register_num & 31) << 11;

// Execute modified instruction
	return rawFunc();
}

void set_cop0_register(unsigned char register_num,
	unsigned int value)
{
// Workaround for MIPS' simplicistic instruction set...	
	unsigned int instr[] =
		{0x40840000, // mtc $a0, 0
		  0x03E00008, // jr $ra
		  0x00000000, // nop
                  0x00000000}; // nop
		  
	void (*rawFunc)(int value) = (void*)instr;

// Write coprocessor register number inside instruction
	instr[0] |= (register_num & 31) << 11;	 

// Execute modified instruction
	rawFunc(value);
}