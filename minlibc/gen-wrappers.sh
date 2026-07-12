#!/bin/sh
# Generate minlibc compiler wrapper scripts
# Usage: gen-wrappers.sh <sysroot> [arch platform compiler ...]
#
# If no arch/platform/compiler args given, generates all combos.
# Otherwise expects triples: arch platform compiler [arch platform compiler ...]

SYSROOT="$1"
shift

ARCHES="x86_64 i386 arm aarch64"
PLATFORMS="linux darwin"
COMPILERS="clang gcc"

if [ $# -eq 0 ]; then
    SETS=""
    for a in $ARCHES; do
        for p in $PLATFORMS; do
            for c in $COMPILERS; do
                SETS="$SETS $a $p $c"
            done
        done
    done
else
    SETS="$@"
fi

mkdir -p "$SYSROOT/bin"

gen_wrapper() {
    _ARCH="$1"
    _PLATFORM="$2"
    _COMPILER="$3"
    _OUT="$SYSROOT/bin/${_ARCH}-${_PLATFORM}-minlibc-${_COMPILER}"

    _EXTRA_CFLAGS=""
    _EXTRA_LINK=""
    if [ "$_PLATFORM" = "darwin" ]; then
        _EXTRA_CFLAGS="-DMINLIBC_DARLING"
        _EXTRA_LINK="-static -Wl,-e,_start"
    fi

    cat > "$_OUT" <<WRAPPER
#!/bin/sh
# minlibc compiler wrapper: ${_ARCH}-${_PLATFORM} via ${_COMPILER}
SYSROOT="\$(cd "\$(dirname "\$0")/.." && pwd)"

COMPILE_ONLY=0
for arg in "\$@"; do
    case "\$arg" in
        -c|-S|-E|-M|-MM|-fsyntax-only) COMPILE_ONLY=1; break;;
    esac
done

if [ "\$COMPILE_ONLY" -eq 1 ]; then
    exec ${_COMPILER} \\
        -I"\${SYSROOT}/include" \\
        ${_EXTRA_CFLAGS} \\
        "\$@"
else
    exec ${_COMPILER} \\
        -nostdlib \\
        -I"\${SYSROOT}/include" \\
        -L"\${SYSROOT}/lib" \\
        "\${SYSROOT}/lib/crt0.o" \\
        -lSystem \\
        ${_EXTRA_CFLAGS} ${_EXTRA_LINK} \\
        "\$@"
fi
WRAPPER

    chmod +x "$_OUT"
    echo "  ${_ARCH}-${_PLATFORM}-minlibc-${_COMPILER}"
}

i=0
for arg in $SETS; do
    i=$((i + 1))
    case $i in
        1) _A="$arg" ;;
        2) _P="$arg" ;;
        3) _C="$arg"
           gen_wrapper "$_A" "$_P" "$_C"
           i=0 ;;
    esac
done
