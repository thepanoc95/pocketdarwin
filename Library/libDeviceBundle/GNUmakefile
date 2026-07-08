# libDeviceBundle - GNU Makefile

CXX := clang++
CXXFLAGS := -std=c++17 -fPIC -Wall -Wextra -I.
LDFLAGS := -shared

# Source files
CPP_SRCS := DeviceInfo.cpp DeviceBundle.cpp DeviceTree.cpp device-bundle-tool.cpp
CC_SRCS := libpbxparser/plist_decoder.cc libpbxparser/value.cc

# Library source files
LIB_CPP_SRCS := DeviceInfo.cpp DeviceBundle.cpp DeviceTree.cpp
LIB_CC_SRCS := libpbxparser/plist_decoder.cc libpbxparser/value.cc

# Objects
LIB_OBJS := $(LIB_CPP_SRCS:.cpp=.o) $(LIB_CC_SRCS:.cc=.o)
TOOL_OBJS := device-bundle-tool.o $(LIB_OBJS)
TEST_OBJS := tests.o $(LIB_OBJS)

# Output
TARGET := libDeviceBundle.so
STATIC_TARGET := libDeviceBundle.a
TOOL := device-bundle-tool
TEST := device-bundle-tests

# Directories
INSTALL_PREFIX ?= /usr/local

.PHONY: all test clean install uninstall

all: $(TARGET) $(STATIC_TARGET) $(TOOL) $(TEST)

$(TARGET): $(LIB_OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

$(STATIC_TARGET): $(LIB_OBJS)
	ar rcs $@ $^

$(TOOL): $(TOOL_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TEST): $(TEST_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $<

test: $(TEST)
	./$(TEST)

clean:
	rm -f $(LIB_OBJS) device-bundle-tool.o tests.o $(TARGET) $(STATIC_TARGET) $(TOOL) $(TEST)

install: $(TARGET) $(STATIC_TARGET) $(TOOL)
	mkdir -p $(INSTALL_PREFIX)/lib
	mkdir -p $(INSTALL_PREFIX)/include/libDeviceBundle
	mkdir -p $(INSTALL_PREFIX)/bin
	cp $(TARGET) $(INSTALL_PREFIX)/lib/
	cp $(STATIC_TARGET) $(INSTALL_PREFIX)/lib/
	cp DeviceInfo.hpp DeviceBundle.hpp DeviceTree.hpp $(INSTALL_PREFIX)/include/libDeviceBundle/
	cp libpbxparser/plist_decoder.hpp libpbxparser/value.hpp $(INSTALL_PREFIX)/include/libDeviceBundle/
	cp $(TOOL) $(INSTALL_PREFIX)/bin/
	cp create-device-bundle.py $(INSTALL_PREFIX)/bin/
	chmod +x $(INSTALL_PREFIX)/bin/create-device-bundle.py

uninstall:
	rm -f $(INSTALL_PREFIX)/lib/$(TARGET)
	rm -f $(INSTALL_PREFIX)/lib/$(STATIC_TARGET)
	rm -f $(INSTALL_PREFIX)/bin/$(TOOL)
	rm -f $(INSTALL_PREFIX)/bin/create-device-bundle.py
	rm -rf $(INSTALL_PREFIX)/include/libDeviceBundle
