include ../deps/makefiles/platform.mak

LIBRARIES += ../src ../deps/stlplus/subsystems ../deps/stlplus/containers ../deps/stlplus/portability

ifeq ($(RELEASE),on)
CXXFLAGS += -ggdb
endif

CXXFLAGS += -Wall -Wextra
CXXFLAGS += -I../include -I../deps/plog/include

ifeq ($(RELEASE),on)
TARGETDIR := ../../bin/release
else
TARGETDIR := ../../bin/debug
endif

IMAGENAME := $(TARGETDIR)/cpp-project
include ../deps/makefiles/gcc.mak
