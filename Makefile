.PHONY: src tests cli gtest deps clean all distclean
all: src cli
gtest:
	$(MAKE) -C tests -f Makefile.gtest RELEASE=$(RELEASE)

tests: gtest src
	$(MAKE) -C tests RELEASE=$(RELEASE)

cli: src deps
	$(MAKE) -C cli RELEASE=$(RELEASE)

deps:
	$(MAKE) -C deps/stlplus/subsystems
	$(MAKE) -C deps/stlplus/portability

src:
	$(MAKE) -C src RELEASE=$(RELEASE)

clean:
	$(MAKE) -C tests -f Makefile.gtest clean RELEASE=$(RELEASE)
	$(MAKE) -C tests clean RELEASE=$(RELEASE)
	$(MAKE) -C cli clean RELEASE=$(RELEASE)
	$(MAKE) -C src clean RELEASE=$(RELEASE)

distclean : clean
	$(MAKE) -C deps/stlplus clean RELEASE=$(RELEASE)
