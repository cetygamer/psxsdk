#!/bin/sh

# This shell script will generate the playstation.x linker script, and the psx-gcc and psx-elf2x shell scripts.

# You have to pass the PREFIX of the toolchain as the first argument of this shell script

echo "/* 
 * Linker script to generate an ELF file
 * that has to be converted to PS-X EXE.
 */

TARGET(\"elf32-littlemips\")
OUTPUT_ARCH(\"mips\")

ENTRY(\"_start\")

SEARCH_DIR(\"$1/lib\")
STARTUP(start.o)
INPUT(-lpsx -lgcc)

SECTIONS
{
	. = 0x80010000;

	.text ALIGN(4) :
	{
		*(.text*)
	}

	.rodata ALIGN(4) :
	{
		*(.rodata)
	}

	.data ALIGN(4) :
	{
		 *(.data)
	}

	.ctors ALIGN(4) :
	{
		*(.ctors)
	}

	.dtors ALIGN(4) :
 	{
		*(.dtors)
	}

	.bss  ALIGN(4) :
	{
		*(.bss)
	}

	__text_start = ADDR(.text);
	__text_end = ADDR(.text) + SIZEOF(.text);

	__rodata_start = ADDR(.rodata);
	__rodata_end = ADDR(.rodata) + SIZEOF(.rodata);

	__data_start = ADDR(.data);
	__data_end = ADDR(.data) + SIZEOF(.data);

	__ctor_list = ADDR(.ctors);
	__ctor_end = ADDR(.ctors) + SIZEOF(.ctors);

	__dtor_list = ADDR(.dtors);
	__dtor_end = ADDR(.dtors) + SIZEOF(.dtors);
	
	__bss_start = ADDR(.bss);
	__bss_end = ADDR(.bss) + SIZEOF(.bss);
	
	__scratchpad = 0x1f800000;
}
" > playstation.x

echo "#!/bin/sh
mipsel-unknown-elf-gcc -D__PSXSDK__ -fno-strict-overflow -fsigned-char -msoft-float -mno-gpopt -fno-builtin -G0 -I$1/include -T $1/mipsel-unknown-elf/lib/ldscripts/playstation.x \$*"> psx-gcc
chmod +x psx-gcc
echo "#!/bin/sh
mipsel-unknown-elf-g++ -D__PSXSDK__ -fno-strict-overflow -fsigned-char -msoft-float -mno-gpopt -fno-builtin -G0 -I$1/include -T $1/mipsel-unknown-elf/lib/ldscripts/playstation.x -fno-rtti -fno-exceptions -fno-threadsafe-statics -fno-use-cxa-atexit \$*" > psx-g++
chmod +x psx-g++
