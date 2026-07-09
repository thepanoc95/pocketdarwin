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
          dtc
          android-tools
          gnumake
        ];

        shellHook = ''
          echo "MegaBoot ARM32 Bootloader"
          echo ""
          echo "Build commands:"
          echo "  make           - Build boot.img (for harpia/msm8916)"
          echo "  make clean     - Clean build artifacts"
          echo ""
          echo "Output: boot.img ready for fastboot boot"
          echo ""
        '';
      };
    });

    formatter = forAllSystems (system: nixpkgs.legacyPackages.${system}.nixpkgs-fmt);
  };
}