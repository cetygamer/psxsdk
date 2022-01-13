# Makefile to build PSXSDK

include Makefile.cfg

build:
	$(MAKE_COMMAND) -C misc SRCROOT=$(PWD)
	sh -c "export PATH=\"$$PATH\":$(TOOLCHAIN_PREFIX)/bin;$(MAKE_COMMAND) -C libpsx SRCROOT=$(PWD)"
	sh -c "export PATH=\"$$PATH\":$(TOOLCHAIN_PREFIX)/bin;$(MAKE_COMMAND) -C libhuff SRCROOT=$(PWD)" 
	sh -c "export PATH=\"$$PATH\":$(TOOLCHAIN_PREFIX)/bin;$(MAKE_COMMAND) -C libm SRCROOT=$(PWD)" 
	sh -c "export PATH=\"$$PATH\":$(TOOLCHAIN_PREFIX)/bin;$(MAKE_COMMAND) -C libadpcm SRCROOT=$(PWD)"
	sh -c "export PATH=\"$$PATH\":$(TOOLCHAIN_PREFIX)/bin;$(MAKE_COMMAND) -C libmodplay SRCROOT=$(PWD)"
	sh -c "export PATH=\"$$PATH\":$(TOOLCHAIN_PREFIX)/bin;$(MAKE_COMMAND) -C libfixmath SRCROOT=$(PWD)"
	$(MAKE_COMMAND) -C tools

install: build
	$(MAKE_COMMAND) -C misc install
	$(MAKE_COMMAND) -C libpsx install
	$(MAKE_COMMAND) -C libhuff install
	$(MAKE_COMMAND) -C libm install
	$(MAKE_COMMAND) -C libadpcm install
	$(MAKE_COMMAND) -C libmodplay install
	$(MAKE_COMMAND) -C libfixmath install
	$(MAKE_COMMAND) -C tools install
	$(MAKE_COMMAND) -C licenses install

clean:
	$(MAKE_COMMAND) -C libpsx clean
	$(MAKE_COMMAND) -C libhuff clean
	$(MAKE_COMMAND) -C libm clean
	$(MAKE_COMMAND) -C libadpcm clean
	$(MAKE_COMMAND) -C libmodplay clean
	$(MAKE_COMMAND) -C libfixmath clean
	$(MAKE_COMMAND) -C misc clean
	$(MAKE_COMMAND) -C tools clean

distclean:
	$(MAKE_COMMAND) -C libpsx distclean
	$(MAKE_COMMAND) -C libhuff clean
	$(MAKE_COMMAND) -C libm clean
	$(MAKE_COMMAND) -C libadpcm clean
	$(MAKE_COMMAND) -C libmodplay clean
	$(MAKE_COMMAND) -C libfixmath clean
	$(MAKE_COMMAND) -C misc distclean
	$(MAKE_COMMAND) -C tools distclean

docs:
	$(DOXYGEN) doxygen.conf

docs_clean:
	rm -fr doc/*
