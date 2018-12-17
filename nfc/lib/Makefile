


# Version numbers
LIBVER_MAJOR_SCRIPT:=`sed -n '/define ZSTD_VERSION_MAJOR/s/.*[[:blank:]]\([0-9][0-9]*\).*/\1/p' < ./zstd.h`
LIBVER_MINOR_SCRIPT:=`sed -n '/define ZSTD_VERSION_MINOR/s/.*[[:blank:]]\([0-9][0-9]*\).*/\1/p' < ./zstd.h`
LIBVER_PATCH_SCRIPT:=`sed -n '/define ZSTD_VERSION_RELEASE/s/.*[[:blank:]]\([0-9][0-9]*\).*/\1/p' < ./zstd.h`
LIBVER_SCRIPT:= $(LIBVER_MAJOR_SCRIPT).$(LIBVER_MINOR_SCRIPT).$(LIBVER_PATCH_SCRIPT)
LIBVER_MAJOR := $(shell echo $(LIBVER_MAJOR_SCRIPT))
LIBVER_MINOR := $(shell echo $(LIBVER_MINOR_SCRIPT))
LIBVER_PATCH := $(shell echo $(LIBVER_PATCH_SCRIPT))
LIBVER := $(shell echo $(LIBVER_SCRIPT))
VERSION?= $(LIBVER)

CPPFLAGS+= -I. -I./common -DXXH_NAMESPACE=ZSTD_
CFLAGS  ?= -O3
DEBUGFLAGS = -Wall -Wextra -Wcast-qual -Wcast-align -Wshadow \
            -Wstrict-aliasing=1 -Wswitch-enum -Wdeclaration-after-statement \
            -Wstrict-prototypes -Wundef -Wpointer-arith -Wformat-security \
            -Wvla -Wformat=2 -Winit-self -Wfloat-equal -Wwrite-strings \
            -Wredundant-decls
CFLAGS  += $(DEBUGFLAGS) $(MOREFLAGS)
FLAGS    = $(CPPFLAGS) $(CFLAGS)




#=============================================================
ZSTDCOMMON_FILES := $(sort $(wildcard common/*.c))
ZSTDCOMP_FILES := $(sort $(wildcard compress/*.c))
ZSTDDECOMP_FILES := $(sort $(wildcard decompress/*.c))
ZDICT_FILES := $(sort $(wildcard dictBuilder/*.c))
ZSTD_FILES := $(ZSTDCOMMON_FILES)


ZSTD_LIB_COMPRESSION ?= 1
ZSTD_LIB_DECOMPRESSION ?= 1
ZSTD_LIB_DICTBUILDER ?= 1


ifeq ($(ZSTD_LIB_COMPRESSION), 0)
	ZSTD_LIB_DICTBUILDER = 0
endif



ifneq ($(ZSTD_LIB_COMPRESSION), 0)
	ZSTD_FILES += $(ZSTDCOMP_FILES) 
endif


ifneq ($(ZSTD_LIB_DECOMPRESSION), 0)
	ZSTD_FILES += $(ZSTDDECOMP_FILES) 
endif



ifneq ($(ZSTD_LIB_DICTBUILDER), 0)
	ZSTD_FILES += $(ZDICT_FILES)
endif



CPPFLAGS  +=

ZSTD_OBJ   := $(patsubst %.c,%.o,$(ZSTD_FILES))


SONAME_FLAGS = -Wl,-soname=libzstd.$(SHARED_EXT).$(LIBVER_MAJOR)
SHARED_EXT = so
SHARED_EXT_MAJOR = $(SHARED_EXT).$(LIBVER_MAJOR)
SHARED_EXT_VER = $(SHARED_EXT).$(LIBVER)


LIBZSTD = libzstd.$(SHARED_EXT_VER)



#=======================================================================================
.PHONY: default all clean

default: lib-release

all: lib

libzstd.a: ARFLAGS = rcs
libzstd.a: $(ZSTD_OBJ)
	@echo compiling static library
	@$(AR) $(ARFLAGS) $@ $^




$(LIBZSTD): LDFLAGS += -shared -fPIC -fvisibility=hidden
$(LIBZSTD): $(ZSTD_FILES)
	@echo compiling dynamic library $(LIBVER)
	@$(CC) $(FLAGS) $^ $(LDFLAGS) $(SONAME_FLAGS) -o $@ -lm
	@echo creating versioned links
	@ln -sf $@ libzstd.$(SHARED_EXT_MAJOR)
	@ln -sf $@ libzstd.$(SHARED_EXT)


libzstd : $(LIBZSTD)



lib: libzstd.a libzstd


lib-release: DEBUGFLAGS :=
lib-release: lib


clean:
	@$(RM) -r *.dSYM   # macOS-specific
	@$(RM) core *.o *.a *.gcda *.$(SHARED_EXT) *.$(SHARED_EXT).* libzstd.pc
	@$(RM) dll/libzstd.dll dll/libzstd.lib
	@$(RM) common/*.o compress/*.o decompress/*.o dictBuilder/*.o
	@echo Cleaning library completed














