include ../../Makefile.cfg

all:
	mkdir -p cd_root
	mkdir -p cdimg
	psx-gcc $(CFLAGS) -DEXAMPLES_VMODE=$(EXAMPLES_VMODE) -o rottest.elf rottest.c
	elf2exe rottest.elf rottest.exe
	cp rottest.exe cd_root
	bmp2tim image.bmp cd_root/image.tim 16 -org=320,0
	systemcnf rottest.exe > cd_root/system.cnf
	$(MKISOFS_COMMAND) -o rottest.hsf -V ROTTEST -sysid PLAYSTATION cd_root
	mkpsxiso rottest.hsf rottest.bin $(CDLIC_FILE)
	mv rottest.bin rottest.cue cdimg
	rm -f rottest.hsf
	rm -fr cd_root
	
clean:
	rm -f rottest.exe rottest.elf
	rm -fr cdimg
	rm -f image.tim