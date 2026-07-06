{
  description = "PocketDarwin - Darwin/XNU on Android devices";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    xnu-deps-linux = {
      url = "github:b-man/xnu-deps-linux/160882ea18c02bf0de28bf9dc230bd2e1e72b4f9";
      flake = false;
    };
  };

  outputs = { self, nixpkgs, xnu-deps-linux }: let
    forAllSystems = nixpkgs.lib.genAttrs [ "x86_64-linux" "aarch64-linux" ];

    # Cross-toolchain: provides aarch64-linux-gnu-{gcc,ld,as,objcopy,...}
    # symlinked from nixpkgs' aarch64-unknown-linux-gnu- prefixed tools.
    crossToolchain = pkgs: let
      cross = pkgs.pkgsCross.aarch64-multiplatform;
    in pkgs.symlinkJoin {
      name = "aarch64-linux-gnu-cross-toolchain";
      paths = [ cross.buildPackages.gcc cross.buildPackages.binutils ];
      postBuild = ''
        for f in $out/bin/aarch64-unknown-linux-gnu-*; do
          name=$(basename "$f")
          new_name=''${name/#aarch64-unknown-linux-gnu-/aarch64-linux-gnu-}
          ln -sf "$f" "$out/bin/$new_name"
        done
      '';
    };

    # Kext host tools: setsegname and kextsymboltool
    kextTools = pkgs: pkgs.stdenv.mkDerivation {
      pname = "kext-tools";
      version = "0.1.0";
      src = "${xnu-deps-linux}/kext-tools";
      nativeBuildInputs = with pkgs; [ clang libuuid ];
      buildPhase = ''
        make -C setsegname
        make -C kextsymboltool
      '';
      installPhase = ''
        mkdir -p $out/bin
        cp setsegname/setsegname $out/bin/
        cp kextsymboltool/kextsymboltool $out/bin/
      '';
    };

    # Build scripts (build-board, build-kext, prepare-board)
    buildScripts = rootDir: pkgs: pkgs.runCommand "pocketdarwin-scripts" {} ''
      mkdir -p $out/bin
      cp ${rootDir}/nix/build-board    $out/bin/build-board
      cp ${rootDir}/nix/build-kext     $out/bin/build-kext
      cp ${rootDir}/nix/prepare-board  $out/bin/prepare-board
      chmod +x $out/bin/*
    '';
  in {
    packages = forAllSystems (system: let
      pkgs = import nixpkgs { inherit system; };
    in {
      dtc              = pkgs.dtc;
      android-tools    = pkgs.android-tools;
      cross-toolchain  = crossToolchain pkgs;
      kext-tools       = kextTools pkgs;
      default          = crossToolchain pkgs;
    });

    devShells = forAllSystems (system: let
      pkgs = import nixpkgs { inherit system; };
    in {
      default = pkgs.mkShell {
        name = "pocketdarwin-dev";

        packages = with pkgs; [
          (crossToolchain pkgs)
          dtc
          android-tools
          python3
          clang
          libuuid
          pkg-config
          git
          gnumake
          cmake
          (kextTools pkgs)
          (buildScripts self.outPath pkgs)
        ];

        shellHook = ''
          export CROSS_COMPILE="aarch64-linux-gnu-"

          echo ""
          echo "  ██████╗  ██████╗  ██████╗██╗  ██╗███████╗████████╗"
          echo "  ██╔══██╗██╔═══██╗██╔════╝██║  ██║██╔════╝╚══██╔══╝"
          echo "  ██████╔╝██║   ██║██║     ███████║█████╗     ██║   "
          echo "  ██╔═══╝ ██║   ██║██║     ██╔══██║██╔══╝     ██║   "
          echo "  ██║     ╚██████╔╝╚██████╗██║  ██║███████╗   ██║   "
          echo "  ╚═╝      ╚═════╝  ╚═════╝╚═╝  ╚═╝╚══════╝   ╚═╝   "
          echo "  Darwin/XNU on Android devices"
          echo ""
          echo "  Available commands:"
          echo "    build-board [options]   Build the full OS (kernel + drivers + boot image)"
          echo "    build-kext  [options]   Build AndroidPlatformDevice.kext only"
          echo "    prepare-board [options] Stage board definition files"
          echo ""
          echo "  Quick start:"
          echo "    build-board                     Build for harpia (Moto G4 Play)"
          echo "    build-board --board-dir ./my-board  Build from custom board dir"
          echo "    build-board --board foo -j\$(nproc)  Parallel build for board 'foo'"
          echo ""
          echo "  CROSS_COMPILE: $CROSS_COMPILE"
          echo "  Built-in board: harpia"
          echo ""
        '';
      };
    });

    formatter = forAllSystems (system: nixpkgs.legacyPackages.${system}.nixpkgs-fmt);
  };
}
