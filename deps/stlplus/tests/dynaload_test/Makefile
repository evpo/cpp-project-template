IMAGE     := dynaload_test
ifeq ($(MONOLITHIC),on)
LIBRARIES := ../../../stlplus3/source
else
LIBRARIES := ../../portability
endif
CPPFLAGS += -DTESTPLATFORM=\"$(SUBDIR)\"
include ../../../makefiles/gcc.mak
ifeq ($(PLATFORM),GNULINUX)
LDLIBS += -ldl
endif


