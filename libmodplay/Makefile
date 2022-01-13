# Makefile for libmodplay

# HOST_* variables specify the programs for compiling code on the host computer

include ../Makefile.cfg

CC = psx-gcc -O0 -g
HOST_CC = gcc -O0 -g
HOST_AR = ar
HOST_RANLIB = ranlib
AR = mipsel-unknown-elf-ar
RANLIB = mipsel-unknown-elf-ranlib

all: libmodplay.a libmodplay_nopsx.a

modplay.o: modplay.c
	$(CC) -c modplay.c
	
mod.o: mod.c
	$(CC) -c mod.c
	
c669.o: c669.c
	$(CC) -c c669.c

mtm.o: mtm.c
	$(CC) -c mtm.c
	
xm.o: xm.c
	$(CC) -c xm.c

libmodplay.a: modplay.o mod.o c669.o mtm.o xm.o
	rm -f libmodplay.a
	$(AR) r libmodplay.a modplay.o mod.o c669.o mtm.o xm.o
	$(RANLIB) libmodplay.a

modplay_nopsx.o: modplay.c
	$(HOST_CC) -DNO_PSX_LIB -c modplay.c -o modplay_nopsx.o

mod_nopsx.o: mod.c
	$(HOST_CC) -DNO_PSX_LIB -c mod.c -o mod_nopsx.o

c669_nopsx.o: c669.c
	$(HOST_CC) -DNO_PSX_LIB -c c669.c -o c669_nopsx.o

mtm_nopsx.o: mtm.c
	$(HOST_CC) -DNO_PSX_LIB -c mtm.c -o mtm_nopsx.o
	
xm_nopsx.o: xm.c
	$(HOST_CC) -DNO_PSX_LIB -c xm.c -o xm_nopsx.o
	
libmodplay_nopsx.a: modplay_nopsx.o mod_nopsx.o c669_nopsx.o mtm_nopsx.o xm_nopsx.o
	rm -f libmodplay_nopsx.a
	$(HOST_AR) r libmodplay_nopsx.a modplay_nopsx.o mod_nopsx.o c669_nopsx.o mtm_nopsx.o xm_nopsx.o
	$(HOST_RANLIB) libmodplay_nopsx.a

install: all
	cp libmodplay.a $(TOOLCHAIN_PREFIX)/lib
	cp modplay.h $(TOOLCHAIN_PREFIX)/include

clean:
	rm -f *.o *.a

distclean: clean
