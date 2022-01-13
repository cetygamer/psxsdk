include ../Makefile.cfg

OBJS =	$(patsubst %.cc, %.o, $(wildcard *.cc))

all: libstdc++.a

libstdc++.a: $(OBJS)
	ar r libstdc++.a $(OBJS)
	
%.o: %.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $<

install: libstdc++.a
	cp libstdc++.a $(TOOLCHAIN_PREFIX)/lib

clean:
	rm -f $(OBJS)

distclean: clean
