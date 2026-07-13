#!/bin/sh

# if there's no uname just print the kernel's build thing
if [ ! command -v uname >/dev/null 2>&1 ]; then
    if [ -f /proc/version ]; then
        cat /proc/version | grep ARCH | cut -d ' ' -f 2
    else
        echo "Unknown"
    fi
    exit 0
fi