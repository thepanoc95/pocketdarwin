# Minimal ARM32-compatible build scaffold for Qualcomm MMC/SDHCI
SHELL := /bin/bash
ROOTDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))/../..)
OUTDIR ?= $(ROOTDIR)/build/qualcommmmc
CXX ?= clang++
CXXFLAGS ?= -O2 -Wall -Wextra -std=gnu++17
OBJS := $(OUTDIR)/AndroidPlatformDevice.o $(OUTDIR)/MSM8916SDHCI.o

.PHONY: all clean

all: $(OUTDIR)/qualcomm_mmc_test.o

$(OUTDIR):
	mkdir -p $@

$(OUTDIR)/AndroidPlatformDevice.o: $(ROOTDIR)/drivers/AndroidPlatformDevice.cpp | $(OUTDIR)
	$(CXX) $(CXXFLAGS) -I$(ROOTDIR)/drivers -c $< -o $@

$(OUTDIR)/MSM8916SDHCI.o: $(ROOTDIR)/drivers/harpia/MSM8916SDHCI.cpp | $(OUTDIR)
	$(CXX) $(CXXFLAGS) -I$(ROOTDIR)/drivers -c $< -o $@

$(OUTDIR)/qualcomm_mmc_test.o: $(OBJS)
	@mkdir -p $(OUTDIR)
	printf '#include <stdint.h>\nint qualcomm_mmc_test(void) { return 0; }\n' | $(CXX) $(CXXFLAGS) -x c++ -c -o $@ -

clean:
	rm -rf $(OUTDIR)
