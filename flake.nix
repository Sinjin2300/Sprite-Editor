{
  description = "Qt6 qmake build for Sprite Editor";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs =
    { nixpkgs, ... }:
    let
      systems = [
        "x86_64-linux"
        "aarch64-linux"
        "x86_64-darwin"
        "aarch64-darwin"
      ];

      forAllSystems = nixpkgs.lib.genAttrs systems;
    in
    {
      packages = forAllSystems (
        system:
        let
          pkgs = import nixpkgs { inherit system; };
        in
        {
          default = pkgs.stdenv.mkDerivation {
            pname = "sprite-editor";
            version = "1.0";

            src = ./.;

            nativeBuildInputs = [
              pkgs.qt6.qmake
              pkgs.qt6.qttools
              pkgs.qt6.wrapQtAppsHook
            ];

            buildInputs = [
              pkgs.qt6.qtbase
            ];

            buildPhase = ''
              qmake6 Sprite_Editor.pro
              make
            '';

            installPhase = ''
              mkdir -p $out/bin
              install -m755 "Sprite_Editor" $out/bin/sprite-editor
            '';
          };
        }
      );
    };
}
