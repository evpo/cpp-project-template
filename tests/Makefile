include ../deps/makefiles/platform.mak

LIBRARIES += ../src

ifeq ($(PLATFORM),MACOS)
LDFLAGS += -Wl,-all_load
LDLIBS += -Wl,-noall_load
else
LDFLAGS += -Wl,--whole-archive
LDLIBS += -Wl,--no-whole-archive
endif

ifeq ($(RELEASE),on)
CXXFLAGS += -ggdb
endif

LDLIBS += ../deps/gtest/googletest/build/libgtest.a -lpthread -lm
CXXFLAGS += -Wall -Wextra -std=c++17
CXXFLAGS += -I../deps/gtest/googletest/include -pthread
CXXFLAGS += -I../include -I../include/internal -I../deps/stlplus/containers -I../deps/plog/include

ifeq ($(RELEASE),on)
TARGETDIR := ../../bin/release
else
TARGETDIR := ../../bin/debug
endif

IMAGENAME := $(TARGETDIR)/cpp-project-tests
include ../deps/makefiles/gcc.mak
