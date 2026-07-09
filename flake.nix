{
  description = "PocketDarwin - Darwin/XNU on Android devices";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs }: let
    forAllSystems = nixpkgs.lib.genAttrs [ "x86_64-linux" "aarch64-linux" ];

    # Cross-toolchain helper: wraps nixpkgs' cross gcc/binutils with
    # a simplified target-triple prefix (e.g. aarch64-linux-gnu-).
    crossToolchain = { prefix, nixpkgsCrossName }: pkgs: let
      cross = nixpkgsCrossName pkgs;
    in pkgs.symlinkJoin {
      name = "${prefix}-cross-toolchain";
      paths = [ cross.buildPackages.gcc cross.buildPackages.binutils ];
      postBuild = ''
        for f in $out/bin/${cross.targetPlatform.config}-*; do
          name=$(basename "$f")
          new_name=''${name/#${cross.targetPlatform.config}-/${prefix}-}
          ln -sf "$f" "$out/bin/$new_name"
        done
      '';
    };

    aarch64Cross = crossToolchain {
      prefix = "aarch64-linux-gnu";
      nixpkgsCrossName = pkgs: pkgs.pkgsCross.aarch64-multiplatform;
    };

    armCross = crossToolchain {
      prefix = "arm-linux-gnueabihf";
      nixpkgsCrossName = pkgs: pkgs.pkgsCross.armv7l-hf-multiplatform;
    };

  in {
    packages = forAllSystems (system: let
      pkgs = import nixpkgs { inherit system; };
    in {
      dtc                       = pkgs.dtc;
      android-tools             = pkgs.android-tools;
      aarch64-linux-gnu-toolchain = aarch64Cross pkgs;
      arm-linux-gnueabihf-toolchain = armCross pkgs;
      default                   = armCross pkgs;
    });

    devShells = forAllSystems (system: let
      pkgs = import nixpkgs { inherit system; };
    in {
      default = pkgs.mkShell {
        name = "megaboot-dev";

        packages = with pkgs; [
          (armCross pkgs)
          (aarch64Cross pkgs)
          clang
          dtc
          android-tools
          gnumake
          pkgs.pkgsi686Linux.glibc
        ];

        shellHook = ''
          export CROSS_COMPILE="arm-linux-gnueabihf-"
          export PATCHES=$(pwd)/PatchFiles
          git submodule update --init
          cd boot/MegaBooter && git apply $PATCHES/MegaBooter.patch
          cd ../../
          cd Kernel && git apply $PATCHES/XNUPatch.patch
          cd ..
          cd BuildHost/xnu-deps-linux && git apply $PATCHES/xnu-deps-linux.patch
          cd ../../
          echo "Done applying patches."
          # Build the lk2nd bootloader for a given target.
          # Usage: build_loader [target] [extra make args...]
          #   target defaults to $TARGET, then msm8916.
          #   ARCH (arm/aarch64) selects the cross-toolchain prefix.
          build_loader() {
      #        local target=${1:-${TARGET:-msm8916}}
      #        local arch=${ARCH:-arm}
      #        local jobs=${NIX_BUILD_CORES:-$(nproc)}
      #        shift 2>/dev/null || true

       #       case "$arch" in
       #           arm|arm32|armv7*) export CROSS_COMPILE="arm-linux-gnueabihf-" ;;
       #           aarch64|arm64)    export CROSS_COMPILE="aarch64-linux-gnu-" ;;
       #           *) echo "build_loader: unknown ARCH '$arch'"; return 1 ;;
       #       esac

        #      make -C boot/MegaBooter -j"$jobs" lk2nd TARGET="$target" "$@"
          }

          # Cross-compile a minimal Mach-O executable for testing the
          # Mach-O loader.  Requires clang (with LLVM aarch64 backend).
          # Usage: build_machoTests [outdir]
          build_machoTests() {
              local outdir=${1:-build/macho-tests}
              mkdir -p "$outdir"

              cat > "$outdir/test.c" <<-EOF
				void __attribute__((naked)) _start(void) {
				    __asm__("mov x0, #0\n\tmov x1, #0\n\tb .");
				}
				EOF

              clang -target aarch64-apple-darwin -nostdlib -Wl,-e,_start \
                  "$outdir/test.c" -o "$outdir/macho_test" 2>&1 || {
                  echo "NOTE: clang targeting Mach-O is experimental on Linux."
                  echo "For a reliable test, create a raw binary instead:"
                  build_raw_test "$outdir"
                  return 1
              }
              echo "Mach-O test binary: $outdir/macho_test"
              file "$outdir/macho_test"

              build_raw_test "$outdir"
          }

          # Create a minimal ELF + raw binary pair for testing the ELF/raw loaders.
          build_raw_test() {
              local outdir=''${1:-build/macho-tests}
              mkdir -p "$outdir"

              cat > "$outdir/test.S" <<-EOF
				.text
				.globl _start
				.type _start, %function
				_start:
				    wfi
				    b _start
				EOF

              ${CROSS_COMPILE}as "$outdir/test.S" -o "$outdir/test.o"
              ${CROSS_COMPILE}ld -Ttext=0x80000000 "$outdir/test.o" -o "$outdir/elf_test"
              ${CROSS_COMPILE}objcopy -O binary "$outdir/elf_test" "$outdir/raw_test.bin"

              echo "ELF test binary: $outdir/elf_test"
              echo "Raw test binary:  $outdir/raw_test.bin"
          }

          echo "=== PocketDarwin development shell ==="
          echo "Available helpers:"
          echo "  build_loader [target]   – build lk2nd (default TARGET=msm8916)"
          echo "  build_machoTests [dir]  – test binaries for the new loaders"
          echo "Variables: CROSS_COMPILE=$CROSS_COMPILE TARGET=''${TARGET:-<unset>}"
        '';
      };
    });

    formatter = forAllSystems (system: nixpkgs.legacyPackages.${system}.nixpkgs-fmt);
  };
}
