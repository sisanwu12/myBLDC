{
  description = "STM32 embedded development environment managed by Nix";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = { nixpkgs, ... }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs {
        inherit system;
        config.allowUnfree = true;
      };
      llvmPkgs = pkgs.llvmPackages;
    in {
      devShells.${system}.default = pkgs.mkShell {
        packages = with pkgs; [
          cmake
          ninja
          gcc-arm-embedded
          openocd
          llvmPkgs.clang
          llvmPkgs.clang-tools
          git
          pkg-config
        ];

        shellHook = ''
          if [ -t 1 ]; then
            echo "STM32 development shell is ready."
            echo "Tools:"
            echo "  cmake              -> $(command -v cmake || echo not-found)"
            echo "  ninja              -> $(command -v ninja || echo not-found)"
            echo "  clang              -> $(command -v clang || echo not-found)"
            echo "  clangd             -> $(command -v clangd || echo not-found)"
            echo "  arm-none-eabi-gcc  -> $(command -v arm-none-eabi-gcc || echo not-found)"
            echo "  arm-none-eabi-gdb  -> $(command -v arm-none-eabi-gdb || echo not-found)"
            echo "  openocd            -> $(command -v openocd || echo not-found)"
          fi
        '';
      };
    };
}
