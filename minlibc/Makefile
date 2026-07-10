CROSS_COMPILE := arm-apple-darwin11

ARCH ?= armv7

AS := $(CROSS_COMPILE)-as
LD := $(CROSS_COMPILE)-ld
CC := clang -target arm-apple-darwin11 -arch $(ARCH)

SRCS := libc.c \
	crt.s
