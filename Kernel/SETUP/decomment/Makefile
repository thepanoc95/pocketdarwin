export MakeInc_cmd=${SRCROOT}/makedefs/MakeInc.cmd
export MakeInc_def=${SRCROOT}/makedefs/MakeInc.def
export MakeInc_rule=${SRCROOT}/makedefs/MakeInc.rule
export MakeInc_dir=${SRCROOT}/makedefs/MakeInc.dir


include $(MakeInc_cmd)
include $(MakeInc_def)

OBJS = decomment.o

CFLAGS = -isysroot $(HOST_SDKROOT) -g -O0 -I$(SOURCE) -I.

WARNFLAGS = -Wall

LDFLAGS = -isysroot $(HOST_SDKROOT)

# Linux host tool compat: getc_unlocked/putchar_unlocked need _DEFAULT_SOURCE
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
override CFLAGS += -std=gnu99 -D_DEFAULT_SOURCE -Wno-implicit-int -Wno-implicit-function-declaration
endif

decomment: $(OBJS)
	$(_v)$(HOST_CC) $(LDFLAGS) -o $@ $^
	@echo HOST_LD $@
	$(_v)$(HOST_CODESIGN) -s - $@
	@echo HOST_CODESIGN $@

.c.o:
	$(_v)$(HOST_CC) $(WARNFLAGS) $(CFLAGS) -c -o $@ $<
	@echo HOST_CC $@

do_build_setup: decomment

include $(MakeInc_rule)
include $(MakeInc_dir)
