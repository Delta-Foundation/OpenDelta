{
    description = "Dev env for Rust, Clang and Ncurses";

    inputs = {
        nixpkgs.url = "github:nixos/nixpkgs/nixpkgs-unstable";
    };

    outputs = { self, nixpkgs, ... }:
        let
            system = "x86_64-linux";
            pkgs = import nixpkgs {
                inherit system;
            };
        in {
            devShells.${system}.default = pkgs.mkShell {
                buildInputs = with pkgs; [
                    clang
                    rustc
                    ncurses
                    pkg-config
                    stdenv.cc.cc
                ];

                # After building dltsh try "make run" for run dltsh
                shellHook = ''
                    echo "[Dev env loaded!]"
                    echo "[building dltsh]: "
                    cd shell/ && make build
                '';
            };
        };
}
