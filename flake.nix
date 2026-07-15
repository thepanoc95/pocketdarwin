{
  description = "PocketDarwin - Darwin/XNU on Android devices";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs }: let
    forAllSystems = nixpkgs.lib.genAttrs [ "x86_64-linux" "aarch64-linux" ];

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

    # ARM bare-metal toolchain for embedded development
    armNoneEabihf = pkgs: pkgs.symlinkJoin {
      name = "arm-none-eabi-toolchain";
      paths = [ pkgs."gcc-arm-embedded" ] ++ (pkgs."gcc-arm-embedded".getPackages or []);
      postBuild = ''
        # Create symlinks with arm-none-eabi- prefix (already present in gcc-arm-embedded)
        # No additional work needed as gcc-arm-embedded already uses arm-none-eabi prefix
      '';
    };

  in {
    packages = forAllSystems (system: let
                  pkgs = import nixpkgs { inherit system; };
                in {
                  dtc                       = pkgs.dtc;
                  android-tools             = pkgs.android-tools;
                  aarch64-linux-gnu-toolchain = aarch64Cross pkgs;
                  arm-linux-gnueabihf-toolchain = armCross pkgs;
                  arm-none-eabi-toolchain = armNoneEabihf pkgs;

                  libfdt                    = pkgs.dtc;
                  default                   = armCross pkgs;
                });

    devShells = forAllSystems (system: let
          pkgs = import nixpkgs { inherit system; };
          libPath = pkgs.lib.makeLibraryPath [ pkgs.dtc ];
        in {
          default = pkgs.mkShell {
            name = "PocketDarwin DevSpace";

            packages = with pkgs; [
                          (armCross pkgs)
                          (aarch64Cross pkgs)
                          (armNoneEabihf pkgs)
                          clang
                          dtc
                          android-tools
                          gnumake
                          pkgs.pkgsi686Linux.glibc
                          kotlin
                        ];

            shellHook = ''
              export CROSS_COMPILE="arm-linux-gnueabihf-"
              export LD_LIBRARY_PATH="${libPath}:$LD_LIBRARY_PATH"
              echo "=== PocketDarwin development shell ==="
              echo "CROSS_COMPILE=$CROSS_COMPILE"
              '';
          };
        });

    formatter = forAllSystems (system: nixpkgs.legacyPackages.${system}.nixpkgs-fmt);
  };
}
