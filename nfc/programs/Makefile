


ZSTDDIR = ../lib


#=======================================================================================
#ifeq ($(shell $(CC) -v 2>&1 | grep -c "gcc version "), 1)
#ALIGN_LOOP = -falign-loops=32
#else
#ALIGN_LOOP =
#endif



#=======================================================================================
CPPFLAGS+= -I$(ZSTDDIR) -I$(ZSTDDIR)/common -I$(ZSTDDIR)/compress \
           -I$(ZSTDDIR)/dictBuilder \
           -DXXH_NAMESPACE=ZSTD_

CFLAGS  ?= -O3

DEBUGFLAGS+=-Wall -Wextra -Wcast-qual -Wcast-align -Wshadow \
            -Wstrict-aliasing=1 -Wswitch-enum -Wdeclaration-after-statement \
            -Wstrict-prototypes -Wundef -Wpointer-arith -Wformat-security \
            -Wvla -Wformat=2 -Winit-self -Wfloat-equal -Wwrite-strings \
            -Wredundant-decls

CFLAGS  += $(DEBUGFLAGS) $(MOREFLAGS)
FLAGS    = $(CPPFLAGS) $(CFLAGS) $(LDFLAGS)


ZSTDCOMMON_FILES := $(ZSTDDIR)/common/*.c
ZSTDCOMP_FILES := $(ZSTDDIR)/compress/*.c
ZSTDDECOMP_FILES := $(ZSTDDIR)/decompress/*.c
ZSTD_FILES := $(ZSTDDECOMP_FILES) $(ZSTDCOMMON_FILES) $(ZSTDCOMP_FILES)
ZDICT_FILES := $(ZSTDDIR)/dictBuilder/*.c
ZSTDDECOMP_O = $(ZSTDDIR)/decompress/zstd_decompress.o


ZSTDLEGACY_FILES :=


# Sort files in alphabetical order for reproducible builds
ZSTDLIB_FILES := $(sort $(wildcard $(ZSTD_FILES)) $(wildcard $(ZSTDLEGACY_FILES)) $(wildcard $(ZDICT_FILES)))


EXT =


VOID = /dev/null



#=======================================================================================
# thread detection
NO_THREAD_MSG := ==> no threads, building without multithreading support
HAVE_PTHREAD := $(shell printf '\#include <pthread.h>\nint main(void) { return 0; }' | $(CC) $(FLAGS) -o have_pthread$(EXT) -x c - -pthread 2> $(VOID) && rm have_pthread$(EXT) && echo 1 || echo 0)
HAVE_THREAD := $(shell [ "$(HAVE_PTHREAD)" -eq "1" -o -n "$(filter Windows%,$(OS))" ] && echo 1 || echo 0)
ifeq ($(HAVE_THREAD), 1)
THREAD_MSG := ==> building with threading support
THREAD_LD := -pthread
else
THREAD_MSG := $(NO_THREAD_MSG)
endif


# zlib detection
NO_ZLIB_MSG := ==> no zlib, building zstd without .gz support
HAVE_ZLIB := $(shell printf '\#include <zlib.h>\nint main(void) { return 0; }' | $(CC) $(FLAGS) -o have_zlib$(EXT) -x c - -lz 2> $(VOID) && rm have_zlib$(EXT) && echo 1 || echo 0)
ifeq ($(HAVE_ZLIB), 1)
ZLIB_MSG := ==> building zstd with .gz compression support
ZLIBCPP = -DZSTD_GZCOMPRESS -DZSTD_GZDECOMPRESS
ZLIBLD = -lz
else
ZLIB_MSG := $(NO_ZLIB_MSG)
endif



#==================================set==================================================
.PHONY: default
default: nfc-release




#=======================================================================================
.PHONY: nfc-release
#nfc-release: DEBUGFLAGS :=
nfc-release: nfc
nfc: nfc.o debug.o file_op.o parse_config.o netflow_v5.o netflow_v9.o extract_flow.o extract_field.o reverse_coding.o delta_coding.o deltaofdelta_coding.o column_split.o rle_coding.o general_coding.o merge_data.o zstd_api.o
	gcc -o $@ $^ ../lib/libzstd.a -lm



#nfc : CPPFLAGS += $(ZLIBCPP)
#nfc : LDFLAGS += $(THREAD_LD) $(ZLIBLD) $(LZMALD) $(LZ4LD)
#nfc : CPPFLAGS +=
#nfc : $(ZSTDLIB_FILES) nfc.o debug.o file_op.o parse_config.o netflow_v5.o netflow_v9.o extract_flow.o extract_field.o reverse_coding.o delta_coding.o deltaofdelta_coding.o column_split.o rle_coding.o general_coding.o merge_data.o
	#@echo "$(THREAD_MSG)"
	#@echo "$(ZLIB_MSG)"
	#gcc -o $@ $^ ../lib/libzstd.a
	#$(CC) $(FLAGS) $^ $(RES_FILE) -o $@$(EXT) $(LDFLAGS)



$(ZSTDDECOMP_O): CFLAGS += $(ALIGN_LOOP)




#=======================================================================================
.PHONY: zstd-release
zstd-release: zstd_test
zstd_test: zstd_test.o zstd_api.o debug.o file_op.o
	gcc -o $@ $^ ../lib/libzstd.a




#=======================================================================================
.PHONY: tools-release
tools-release: extract_flow extract_field reverse_coding delta_coding deltaofdelta_coding column_split rle_coding general_coding merge_data

extract_flow: extract_flow_main.o extract_flow.o debug.o file_op.o
	gcc -o $@ $^

extract_field: extract_field_main.o extract_field.o debug.c file_op.o parse_config.o netflow_v5.o netflow_v9.o
	gcc -o $@ $^

reverse_coding: reverse_coding_main.o reverse_coding.o debug.o file_op.o  parse_config.o netflow_v5.o netflow_v9.o
	gcc -o $@ $^


delta_coding: delta_coding_main.o delta_coding.o debug.o file_op.o
	gcc -o $@ $^

deltaofdelta_coding: deltaofdelta_coding_main.o deltaofdelta_coding.o debug.o file_op.o
	gcc -o $@ $^

column_split: column_split_main.o column_split.o debug.o file_op.o netflow_v5.o netflow_v9.o
	gcc -o $@ $^

rle_coding: rle_coding_main.o rle_coding.o debug.o file_op.o
	gcc -o $@ $^


general_coding: general_coding_main.o general_coding.o debug.o file_op.o zstd_api.o
	gcc -o $@ $^ ../lib/libzstd.a

merge_data: merge_data_main.o merge_data.o debug.o file_op.o
	gcc -o $@ $^	


approximation_coding: approximation_coding_main.o approximation_coding.o debug.o file_op.o
	gcc -o $@ $^


#=======================================================================================
.PHONY: clean
clean:
	#$(MAKE) -C $(ZSTDDIR) clean
	@$(RM) $(ZSTDDIR)/decompress/*.o $(ZSTDDIR)/decompress/zstd_decompress.gcda
	@$(RM) core *.o tmp* result* *.gcda dictionary *.zst nfc$(EXT) nfc32$(EXT) nfc-compress$(EXT) nfc-decompress$(EXT) nfc-small$(EXT) nfc-frugal$(EXT) nfc-nolegacy$(EXT) nfc4$(EXT) *.gcda default.profraw have_zlib$(EXT) extract_flow extract_field reverse_coding delta_coding deltaofdelta_coding column_split rle_coding general_coding merge_data
	@echo Cleaning completed
	@echo $(ALIGN_LOOP)










