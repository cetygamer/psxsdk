# Makefile for PSX SDK library

include ../Makefile.cfg

CC = mipsel-unknown-elf-gcc
CFLAGS = -Iinclude/ -G0 -O0 -fno-builtin -mno-gpopt -nostdlib -msoft-float -g -Wall
AS = mipsel-unknown-elf-as
LD = mipsel-unknown-elf-ld
AR = mipsel-unknown-elf-ar
RANLIB = mipsel-unknown-elf-ranlib
OBJS = out/syscalls.o out/psxsdk.o out/libc.o out/setup.o \
	out/memory.o out/gpu.o out/memcard.o out/pad.o out/spu.o out/util.o \
	out/cdrom.o out/cdromh.o out/cop0.o\
	out/libc/printf.o out/libc/string.o out/libc/scanf.o out/libc/qsort.o \
	out/libc/misc.o \
	out/runexe/runexe.o out/runexe/stage2.o \
	start.o

all: libpsx.a

libpsx.a: $(OBJS)
	$(AR) r libpsx.a $(OBJS)
	$(RANLIB) libpsx.a

out/syscalls.o: src/syscalls.s
	$(CC) $(CFLAGS) -c -o out/syscalls.o src/syscalls.s

out/psxsdk.o: src/psxsdk.c
	$(CC) $(CFLAGS) -c -o out/psxsdk.o src/psxsdk.c
	
out/libc.o: src/libc.c
	$(CC) $(CFLAGS) -c -o out/libc.o src/libc.c

out/setup.o: src/setup.c
	$(CC) $(CFLAGS) -c -o out/setup.o src/setup.c

out/memory.o: src/memory.c
	$(CC) $(CFLAGS) -c -o out/memory.o src/memory.c

out/gpu.o: src/gpu.c
	$(CC) $(CFLAGS) -c -o out/gpu.o src/gpu.c

out/memcard.o: src/memcard.c
	$(CC) $(CFLAGS) -c -o out/memcard.o src/memcard.c

out/pad.o: src/pad.c
	$(CC) $(CFLAGS) -c -o out/pad.o src/pad.c

out/spu.o: src/spu.c
	$(CC) $(CFLAGS) -c -o out/spu.o src/spu.c
	
out/util.o: src/util.c
	$(CC) $(CFLAGS) -c -o out/util.o src/util.c

out/cdrom.o: src/cdrom.c
	$(CC) $(CFLAGS) -c -o out/cdrom.o src/cdrom.c

out/cdromh.o: src/cdromh.s
	$(CC) $(CFLAGS) -c -o out/cdromh.o src/cdromh.s

out/cop0.o: src/cop0.c
	$(CC) $(CFLAGS) -c -o out/cop0.o src/cop0.c

out/libc/printf.o: src/libc/printf.c
	$(CC) $(CFLAGS) -c -o out/libc/printf.o src/libc/printf.c

out/libc/string.o: src/libc/string.c
	$(CC) $(CFLAGS) -c -o out/libc/string.o src/libc/string.c

out/libc/scanf.o: src/libc/scanf.c
	$(CC) $(CFLAGS) -c -o out/libc/scanf.o src/libc/scanf.c
	
out/libc/qsort.o: src/libc/qsort.c
	$(CC) $(CFLAGS) -c -o out/libc/qsort.o src/libc/qsort.c

out/libc/misc.o: src/libc/misc.c
	$(CC) $(CFLAGS) -c -o out/libc/misc.o src/libc/misc.c

out/runexe/runexe.o: src/runexe/runexe.c
	$(CC) $(CFLAGS) -c -o out/runexe/runexe.o src/runexe/runexe.c
	
out/runexe/stage2.o: src/runexe/stage2.s
	$(CC) $(CFLAGS) -c -o out/runexe/stage2.o src/runexe/stage2.s

start.o: src/start.s
	$(CC) $(CFLAGS) -c -o start.o src/start.s

install:
	cp libpsx.a $(TOOLCHAIN_PREFIX)/lib
	cp start.o $(TOOLCHAIN_PREFIX)/lib
	cp -r include/*	$(TOOLCHAIN_PREFIX)/include
	
clean:
	rm -f $(OBJS)
	rm -f libpsx.a
	rm -f start.o

distclean: clean
