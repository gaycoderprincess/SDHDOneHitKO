# SDHD One-Hit-KO

1HP/One-Hit-KO mod for Sleeping Dogs: Definitive Edition

https://github.com/gaycoderprincess/SDHDOneHitKO/assets/163012979/1c51829e-f6b4-425b-8be0-fa70f7f71901

## Installation

- Make sure you have the latest Steam version of the game, as this is the only version this plugin is compatible with. (exe size of 37490688 bytes)
- Plop the files into your game folder, edit `SDHDOneHitKO_gcp.toml` to change the options to your liking.
- Enjoy, nya~ :3

## Feature List

- 1HP mode, any damage from any source will kill you
- Small tweaks added to fix certain impossible missions (curse you, grapple system!)
- Options to switch between a full health display, an upgraded health display, and showing zero health with the red screen overlay

## Building

Building is done on an Arch Linux system with CLion and vcpkg being used for the build process. 

Before you begin, clone [nya-common](https://github.com/gaycoderprincess/nya-common) to a folder next to this one, so it can be found.

Required packages: `mingw-w64-gcc vcpkg`

To install all dependencies, use:
```console
vcpkg install tomlplusplus:x86-mingw-static
```

Once installed, copy files from `~/.vcpkg/vcpkg/installed/x64-mingw-static/`:

- `include` dir to `nya-common/3rdparty`
- `lib` dir to `nya-common/lib64`

You should be able to build the project now in CLion.
