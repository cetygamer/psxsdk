# Makefile to build PSXSDK

include Makefile.cfg

ifeq ($(ENABLE_CXX), yes)
	BUILD_CXX = $(HOST_SHELL) -c "export PATH=\"$$PATH\":$(TOOLCHAIN_PREFIX)/bin;$(MAKE_COMMAND) -C cxx SRCROOT=$(PWD)"
	INSTALL_CXX = $(MAKE_COMMAND) -C cxx install
	CLEAN_CXX = $(MAKE_COMMAND) -C cxx clean
endif

build:
	$(MAKE_COMMAND) -C misc SRCROOT=$(PWD)
	$(HOST_SHELL) -c "export PATH=\"$$PATH\":$(TOOLCHAIN_PREFIX)/bin;$(MAKE_COMMAND) -C libpsx SRCROOT=$(PWD)"
	$(HOST_SHELL) -c "export PATH=\"$$PATH\":$(TOOLCHAIN_PREFIX)/bin;$(MAKE_COMMAND) -C libhuff SRCROOT=$(PWD)" 
	$(HOST_SHELL) -c "export PATH=\"$$PATH\":$(TOOLCHAIN_PREFIX)/bin;$(MAKE_COMMAND) -C libm SRCROOT=$(PWD)" 
	$(HOST_SHELL) -c "export PATH=\"$$PATH\":$(TOOLCHAIN_PREFIX)/bin;$(MAKE_COMMAND) -C libadpcm SRCROOT=$(PWD)"
	$(HOST_SHELL) -c "export PATH=\"$$PATH\":$(TOOLCHAIN_PREFIX)/bin;$(MAKE_COMMAND) -C libmodplay SRCROOT=$(PWD)"
	$(HOST_SHELL) -c "export PATH=\"$$PATH\":$(TOOLCHAIN_PREFIX)/bin;$(MAKE_COMMAND) -C libfixmath SRCROOT=$(PWD)"
	$(HOST_SHELL) -c "export PATH=\"$$PATH\":$(TOOLCHAIN_PREFIX)/bin;$(MAKE_COMMAND) -C libf3m SRCROOT=$(PWD)"
	$(HOST_SHELL) -c "export PATH=\"$$PATH\":$(TOOLCHAIN_PREFIX)/bin;$(MAKE_COMMAND) -C libmeidogte SRCROOT=$(PWD)"
	$(BUILD_CXX)
	$(MAKE_COMMAND) -C tools
	@echo Done.

install: build
	$(MAKE_COMMAND) -C misc install
	$(MAKE_COMMAND) -C libpsx install
	$(MAKE_COMMAND) -C libhuff install
	$(MAKE_COMMAND) -C libm install
	$(MAKE_COMMAND) -C libadpcm install
	$(MAKE_COMMAND) -C libmodplay install
	$(MAKE_COMMAND) -C libfixmath install
	$(MAKE_COMMAND) -C libf3m install
	$(MAKE_COMMAND) -C libmeidogte install
	$(MAKE_COMMAND) -C tools install
	$(MAKE_COMMAND) -C licenses install
	$(INSTALL_CXX)

clean: docs_clean
	$(MAKE_COMMAND) -C libpsx clean
	$(MAKE_COMMAND) -C libhuff clean
	$(MAKE_COMMAND) -C libm clean
	$(MAKE_COMMAND) -C libadpcm clean
	$(MAKE_COMMAND) -C libmodplay clean
	$(MAKE_COMMAND) -C libfixmath clean
	$(MAKE_COMMAND) -C libf3m clean
	$(MAKE_COMMAND) -C libmeidogte clean
	$(MAKE_COMMAND) -C misc clean
	$(MAKE_COMMAND) -C tools clean
	$(MAKE_COMMAND) -C examples clean
	$(CLEAN_CXX)

distclean: docs_clean
	$(MAKE_COMMAND) -C libpsx distclean
	$(MAKE_COMMAND) -C libhuff clean
	$(MAKE_COMMAND) -C libm clean
	$(MAKE_COMMAND) -C libadpcm clean
	$(MAKE_COMMAND) -C libmodplay clean
	$(MAKE_COMMAND) -C libfixmath clean
	$(MAKE_COMMAND) -C libf3m clean
	$(MAKE_COMMAND) -C libmeidogte clean
	$(MAKE_COMMAND) -C misc distclean
	$(MAKE_COMMAND) -C tools distclean
	$(MAKE_COMMAND) -C examples distclean
	$(CLEAN_CXX)
	
build_examples:
	$(MAKE_COMMAND) -C examples

docs:
	$(DOXYGEN) doxygen.conf

docs_clean:
	rm -fr doc/*
