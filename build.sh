#!/bin/sh
#
# PocketDarwin Build Script
#
# A convenience wrapper around the top-level Makefile.
# Usage: ./build.sh [options] <target>
#
# Options:
#   -b <board>   Target board (default: harpia)
#   -a <arch>    Architecture (default: arm64)
#   -t <prefix>  Toolchain prefix (default: aarch64-linux-gnu-)
#   -c <config>  Kernel config: RELEASE, DEVELOP, DEBUG (default: RELEASE)
#   -j <n>       Parallel jobs
#   -v           Verbose
#   -h           Show this help
#

set -e

BOARD="harpia"
ARCH="arm64"
CROSS_COMPILE="aarch64-linux-gnu-"
CONFIG="RELEASE"
JOBS=""
VERBOSE=""
TARGET=""

while getopts "b:a:t:c:j:vh" opt; do
    case "$opt" in
        b) BOARD="$OPTARG" ;;
        a) ARCH="$OPTARG" ;;
        t) CROSS_COMPILE="$OPTARG" ;;
        c) CONFIG="$OPTARG" ;;
        j) JOBS="-j$OPTARG" ;;
        v) VERBOSE="V=1" ;;
        h)
            sed -n '2,20p' "$0"
            exit 0
            ;;
        *) exit 1 ;;
    esac
done
shift $((OPTIND - 1))
TARGET="$*"
[ -z "$TARGET" ] && TARGET="all"

# Check toolchain
echo "PocketDarwin Build"
echo "  Board:        $BOARD"
echo "  Arch:         $ARCH"
echo "  Config:       $CONFIG"
echo "  Toolchain:    $CROSS_COMPILE"
echo "  Target:       $TARGET"
echo ""

TOOLCHAIN_CHECK="${CROSS_COMPILE}gcc"
if ! which "$TOOLCHAIN_CHECK" >/dev/null 2>&1; then
    echo "Warning: $TOOLCHAIN_CHECK not found in PATH."
    echo "Install the toolchain or set CROSS_COMPILE manually."
    echo ""
fi

MAKE_ARGS="BOARD=$BOARD ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE KERNEL_CONFIG=$CONFIG"
[ -n "$JOBS" ] && MAKE_ARGS="$MAKE_ARGS $JOBS"
[ -n "$VERBOSE" ] && MAKE_ARGS="$MAKE_ARGS $VERBOSE"

# Run make
exec make $MAKE_ARGS $TARGET
