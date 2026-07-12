#
# mindyld Test Suite Makefile
#

# ============================================================================
# Configuration
# ============================================================================

CC := gcc
CFLAGS := -Wall -Wextra -fPIC -I.. -I../../minlibc/include

# Detect platform
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
    SO_EXT := .dylib
else
    SO_EXT := .so
endif

# ============================================================================
# Targets
# ============================================================================

.PHONY: all clean test test-basic test-advanced test-helper

# Default target
all: test_basic test_advanced test_helper$(SO_EXT)

# ============================================================================
# Test Programs
# ============================================================================

test_basic: test_basic.c
	@echo "[CC] Building basic test..."
	@$(CC) $(CFLAGS) -o $@ $^ -L.. -lmindyld

test_advanced: test_advanced.c
	@echo "[CC] Building advanced test..."
	@$(CC) $(CFLAGS) -o $@ $^ -L.. -lmindyld

# ============================================================================
# Test Helper Library
# ============================================================================

test_helper$(SO_EXT): test_helper_lib.c
	@echo "[CC] Building test helper library..."
	@$(CC) $(CFLAGS) -shared -o $@ $^

# ============================================================================
# Running Tests
# ============================================================================

test: all
	@echo ""
	@echo "Running mindyld Tests"
	@echo "===================="
	@echo ""
	@echo "Test 1: Basic Functionality"
	@echo "--------------------------"
	@-./test_basic
	@echo ""
	@echo "Test 2: Advanced Features"
	@echo "------------------------"
	@-./test_advanced
	@echo ""

test-basic: test_basic
	@echo "Running basic tests..."
	@./test_basic

test-advanced: test_advanced
	@echo "Running advanced tests..."
	@./test_advanced

# ============================================================================
# Cleanup
# ============================================================================

clean:
	@echo "[CLEAN] Removing test artifacts..."
	@rm -f test_basic test_advanced test_helper.so test_helper.dylib *.o

distclean: clean
	@echo "[DISTCLEAN] Complete cleanup"
	@rm -f *~ *.bak *.orig

# ============================================================================
# Help
# ============================================================================

help:
	@echo "mindyld Test Suite"
	@echo "=================="
	@echo ""
	@echo "Targets:"
	@echo "  all          - Build all tests and helper library"
	@echo "  test         - Run all tests"
	@echo "  test-basic   - Run basic tests only"
	@echo "  test-advanced- Run advanced tests only"
	@echo "  clean        - Remove test artifacts"
	@echo "  distclean    - Remove all generated files"
	@echo "  help         - Show this help message"
