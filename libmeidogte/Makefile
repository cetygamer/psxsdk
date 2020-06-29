include ../Makefile.cfg

CFILES	= $(notdir $(wildcard ./*.c))
AFILES	= $(notdir $(wildcard ./*.s))
OFILES	= $(CFILES:.c=.o) $(AFILES:.s=.o)

TARGET	= libmeidogte.a

all: $(TARGET)

$(TARGET): $(OFILES)
	$(AR) cr $(TARGET) $(OFILES)
	$(RANLIB) $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -I./ -c $< -o $@
	
%.o: %.s
	$(AS) $(AFLAGS) -I ./ $< -o $@

clean:
	rm -Rf $(TARGET) $(OFILES)

install: all
	cp $(TARGET) $(TOOLCHAIN_PREFIX)/lib
	cp meidogte.h meidogte_inline.h $(TOOLCHAIN_PREFIX)/include
