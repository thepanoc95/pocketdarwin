{
  description = "PocketDarwin - Darwin/XNU on Android devices";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs }: let
    forAllSystems = nixpkgs.lib.genAttrs [
      "x86_64-linux"
      "aarch64-linux"
    ];

    crossToolchain = { prefix, nixpkgsCrossName }: pkgs: let
      cross = nixpkgsCrossName pkgs;
    in pkgs.symlinkJoin {
      name = "${prefix}-cross-toolchain";
      paths = [
        cross.buildPackages.gcc
        cross.buildPackages.binutils
      ];

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

    armNoneEabihf = pkgs:
      pkgs.symlinkJoin {
        name = "arm-none-eabi-toolchain";

        paths =
          [ pkgs.gcc-arm-embedded ]
          ++ (pkgs.gcc-arm-embedded.getPackages or []);

        postBuild = "";
      };

  in {

    packages = forAllSystems (system:
      let
        pkgs = import nixpkgs {
          inherit system;
        };
      in {
        dtc = pkgs.dtc;
        android-tools = pkgs.android-tools;

        aarch64-linux-gnu-toolchain = aarch64Cross pkgs;
        arm-linux-gnueabihf-toolchain = armCross pkgs;
        arm-none-eabi-toolchain = armNoneEabihf pkgs;

        libfdt = pkgs.dtc;

        default = armCross pkgs;
      });

    devShells = forAllSystems (system:
      let
        pkgs = import nixpkgs {
          inherit system;
        };

        libPath = pkgs.lib.makeLibraryPath [
          pkgs.dtc
          pkgs.openssl
          pkgs.db
          pkgs.libblocksruntime
        ];
      in {
        default = pkgs.mkShell {
          name = "PocketDarwin DevSpace";

          packages = with pkgs; [

            # Cross toolchains
            (armCross pkgs)
            (aarch64Cross pkgs)
            (armNoneEabihf pkgs)
            clang
            llvmPackages.llvm.dev
            gcc
            gcc_multi
            binutils
            gnumake
            pkg-config
            flex
            bison
            automake
            autogen
            libtool
            perl
            tcsh
            dpkg
            devscripts
            debhelper
            db
            openssl.dev
            libuuid
            libblocksruntime
            dtc
            android-tools
            kotlin
            pkgsi686Linux.glibc
            pkgsi686Linux.glibc.dev
          ];

          shellHook = ''
            export CROSS_COMPILE=arm-linux-gnueabihf-
            export LD_LIBRARY_PATH="${libPath}:$LD_LIBRARY_PATH"

            echo "========================================="
            echo " PocketDarwin Development Shell"
            echo "========================================="
            echo "CROSS_COMPILE=$CROSS_COMPILE"
            echo "CC=$(command -v clang)"
            echo "ARM GCC=$(command -v arm-linux-gnueabihf-gcc)"
            echo "ARM EABI GCC=$(command -v arm-none-eabi-gcc)"
            echo "========================================="
          '';
        };
      });

    formatter = forAllSystems (
      system: nixpkgs.legacyPackages.${system}.nixpkgs-fmt
    );
  };
}