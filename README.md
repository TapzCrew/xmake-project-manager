[![CI](https://github.com/Arthapz/xmake-project-manager/actions/workflows/build_cmake.yml/badge.svg)](https://github.com/Arthapz/xmake-project-manager/actions/workflows/build_cmake.yml)

!! This project is in early stage, do not use in production !!

This is some basic XMake build system integration for QtCreator (based on [QtCreator meson plugin](https://github.com/qt-creator/qt-creator/tree/master/src/plugins/mesonprojectmanager) implementation).

Currently implemented features:
- Show files in project:
  - Subprojects
  - CMake plugin like file view
- Basic build integration (XMake default backend only)
- Extract C++ code complemtion information from build directory
- Group files by target
- Support of build modes (debug, etc... ; build modes need to be enabled in your xmake.lua)
    
Not yet implemented:
- xmake.lua code completion
- Non-source file listing

# ArchLinux
You can install this plugin from AUR with git and makepkg
```
sudo pacman -S base-devel cmake
git clone https://aur.archlinux.org/qtcreator-xmake-project-manager.git
cd qtcreator-xmake-project-manager
makepkg -si
```

or from your AUR helper

```
paru/yay/yaourt -S qtcreator-xmake-project-manager
``` 

# Building from source
Please note that the build was only tested with Qt Creator 6.0.1 and other versions most likely won't work as the Qt Creator APIs tend to change even between minor version.
To build the plugin you need the qtcreator headers and libraries. So the safest way is to install them from your distribution package manager ([qtcreator-devel](https://aur.archlinux.org/packages/qtcreator-devel/) from AUR on archlinux), or from the Qt installer

```
git clone https://github.com/Arthapz/xmake-project-manager.git
cd xmake-project-manager
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE:STRING=Release ..
cmake --build .
cmake --install .
```

# Snapshots
You can find automatically built snapshots on [Nightly.link](https://nightly.link/Arthapz/xmake-project-manager/workflows/build_cmake/main).

Uncompress 7z file, open QtCreator and use QtCreator plugin installer

![First-step](https://github.com/Arthapz/xmake-project-manager/blob/screenshots/40-24-22-194046.png?raw=true)
![Second-step](https://github.com/Arthapz/xmake-project-manager/blob/screenshots/40-24-22-194059.png?raw=true)
![Third-step](https://github.com/Arthapz/xmake-project-manager/blob/screenshots/41-24-22-194139.png?raw=true)

Restart QtCreator and enable plugin

![Fourth-step](https://github.com/Arthapz/xmake-project-manager/blob/screenshots/42-24-22-194223.png?raw=true)

Restart a last time QtCreator and Open your XMake project :)
