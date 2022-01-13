include ../../Makefile.cfg

all:
	mkdir -p cd_root
	psx-gcc -O3 $(CFLAGS) -DEXAMPLES_VMODE=$(EXAMPLES_VMODE) -o mandel.elf mandel.c
	elf2exe mandel.elf mandel.exe
	cp mandel.exe cd_root
	systemcnf mandel.exe > cd_root/system.cnf
	$(MKISOFS_COMMAND) -o mandel.hsf -V MANDEL -sysid PLAYSTATION cd_root
	mkpsxiso mandel.hsf mandel.bin $(CDLIC_FILE)
	rm -f mandel.hsf
	
clean:
	rm -f mandel.bin mandel.cue mandel.exe mandel.elf
	rm -fr cd_root
