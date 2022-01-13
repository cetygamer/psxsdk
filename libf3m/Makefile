# Makefile for libf3m

include ../Makefile.cfg

all: libf3m.a

f3m.o: f3m.c
	$(CC) $(CFLAGS) -c f3m.c

libf3m.a: f3m.o
	rm -f libf3m.a
	$(AR) r libf3m.a f3m.o
	$(RANLIB) libf3m.a	

install: all
	cp libf3m.a $(TOOLCHAIN_PREFIX)/lib
	cp f3m.h $(TOOLCHAIN_PREFIX)/include

clean:
	rm -f *.o *.a