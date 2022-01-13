include ../../Makefile.cfg

all:
	mkdir -p cd_root
	bmp2tim puzzle.bmp cd_root/puzzle.tim 16 -org=320,0
	psx-gcc -O3 -DEXAMPLES_VMODE=$(EXAMPLES_VMODE) -o puzzle.elf puzzle.c -Wall
	elf2exe puzzle.elf cd_root/puzzle.exe
	systemcnf puzzle.exe > cd_root/system.cnf
	$(MKISOFS_COMMAND) -o puzzle.hsf -V PUZZLE -sysid PLAYSTATION cd_root
	mkpsxiso puzzle.hsf puzzle.bin /usr/local/psxsdk/share/licenses/infoeur.dat
	rm -f puzzle.hsf

clean:
	rm -fr cd_root puzzle.bin puzzle.cue puzzle.elf 