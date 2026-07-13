CC      ?= gcc
CFLAGS  := -Wall -Wextra -O2 -g -std=c11
LDFLAGS :=

SRCS := main.c cpu.c platform.c
OBJS := $(SRCS:.c=.o)
ifdef ARCH
ifdef ARCH = x86_64
TARGET := msm8simu.elf64
else
TARGET := msm8simu.elf
endif
else
TARGET := msm8simu.elf
endif

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c cpu.h platform.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
