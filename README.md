# Parallel Launcher

An open source launcher for the ParallelN64 and Mupen64Plus-Next emulators featuring very easy controller configuration, automatic loading of ROMs from given folders, and automatic setup of optimal settings.

The ParallelN64 core for RetroArch has a number of advantages over other emulators: it's faster and more accurate than Project64, it comes with all graphics plugins built in so you don't need to manually install and update them, and it runs on Windows, Linux, and Mac. However, as a RetroArch core, it also has some downsides: setting up controllers in RetroArch is needlessly confusing and difficult, you need to change the default settings for the best experience, and it doesn't have a great way to manage your library of games.

Parallel Launcher aims to fix all of these problems, so you can get the best of Nintendo 64 emulation without all the hastle. Parallel Launcher features a very straight-forward controller setup, shows you the important options with good defaults enabled by default, and has a simple game library interface that allows you to categorize your games and track your play time.

Parallel Launcher also features optional integration with romhacking.com. With this integration, you can download hacks from your playlists (with the recommended graphics plugin and other settings set automatically for you), as well as rate hacks and manage your playlists from within the app. It also features a built-in star display for SM64 hacks to track your progress in each hack.

Parallel Launcher also uses a custom build of ParallelN64 that adds some features, including the addition of the GLideN64 plugin and support for emulated Gamecube controllers (yes, the N64 technically supports that!)

### Special Thanks
**[aglab2](https://github.com/aglab2)** for bringing GLideN64 to the ParallelN64 core and providing some optional tweaks to improve compatibility with older romhacks, as well as getting the ParaLLEl plugin working on MacOS and supporting ARM.  
**[Wiseguy](https://github.com/Mr-Wiseguy)** for implementing Gamecube support into the ParallelN64 core and helping with adding that support to Parallel Launcher  
**[devwizard](https://gitlab.com/devwizard64)** for adding SummerCart SD card support to ParallelN64  

### Thanks to the translators:
**Italian** - Luigi "JakeDower" Orabona  
**Spanish** - Daniel "Reonu" León  
**German** - [Mathis "CommandMC" Dröge](https://github.com/CommandMC)  
**French (Standard)** - Naeso  
**French (Québécois)** - max954  
**Arabic** - Elazul  
**Portuguese** - Lila  

## Building Parallel Launcher from Source

### Building on Linux

- Install the dev dependencies.
  - Debian/Ubuntu/Mint: `sudo apt install build-essential qt5-qmake qtdeclarative5-dev qttools5-dev-tools libqt5svg5-dev libsdl2-dev libsqlite3-dev findutils xdg-utils policykit-1 libgcrypt20-dev dosfstools`
  - OpenSUSE: `sudo zypper install gcc9 gcc9-c++ libqt5-qtbase-common-devel libqt5-qtdeclarative-devel libqt5-linguist libSDL2-devel sqlite-devel findutil xdg-utils polkit libgcrypt20 dosfstools`
  - Fedora/CentOS: `sudo yum install gcc9 gcc9-c++ libqt5-qtbase-common-devel libqt5-qtdeclarative-devel libqt5-linguist libSDL2-devel sqlite-devel findutils xdg-utils polkit libgcrypt dosfstools`
  - Arch/Manjaro: `sudo pacman -S gcc glibc qt5-declarative qt5-base qt5-svg qt5-tools make sdl2 findutils xdg-utils polkit sqlite libgcrypt dosfstools`
- Build the makefile with `./qmake-release.sh` (or `./qmake-debug.sh` for the debug build)
- Run `make` to build

If you are editing the UI, you will also want to install either Qt Designer or Qt Creator

### Building on Windows

#### Setup
Note: if you plan on building both 64-bit and 32-bit builds, I recommend that you create both a `release64` and `release32` folder, then copy the appropriate one to the mentioned `release` folder when you want to build.  

- Install Visual Studio 2019
- Install "Desktop development with C++" from the Visual Studio installer
- Download Qt from https://download.qt.io/archive/qt/5.12/5.12.12/qt-opensource-windows-x86-5.12.12.exe
- Run the installer and select a custom install
  - Select the following options from Qt 5.15.2 to install:
    - MSVC 2019 32-bit
    - MSVC 2019 64-bit
    - Qt Network Authorization
    - Qt Debug Information Files
  - Select the following options from Developer and Designer Tools
    - Qt Creator 9.0.2
    - Qt Creator 9.0.2 CDB Debugger Support
    - Debugging Tools for Windows
    - OpenSSL 1.1.1q Toolkit
- In Qt Creator, go to Tools -> Options, then set your cmake location if it was not set automatically
- Create a `win64` folder (or `win32` for building 32-bit) in your checkout directory and put an `include` and `lib` directory inside it.
- Copy `kernel32.lib`, `Ole32.lib`, `AdvAPI32.lib`, `dinput8.lib`, `dxguid.lib`, `DbgHelp.Lib`, `User32.lib`, `Version.lib`, `Shell32.lib`, and `Virtdisk.lib` to your `lib` directory (should be found somewhere like `C:\Program Files (x86)\Windows Kits\10\Lib\10.0.18362.0\um\x64`)
- Copy `dbghelp.dll` from `C:\Program Files (x86)\Windows Kits\10\Debuggers\x64` (or `\x86` for 32-bit) to your `release` folder.
- Copy `msvcr100.dll` from `C:\Windows\System32` to your `release` folder. (Or, if you are doing a 32-bit build on a 64-bit machine, take the one in the `C:\Windows\SysWOW64` folder)
- Download the SDL2 development libraries for Visual C++ from https://github.com/libsdl-org/SDL/releases
  - Copy the `SDL2.lib` file into your `win64/lib` (or `win32/lib`) folder
  - Make an `SDL2` directory in your `win64/include` or `win32/include` directory, and copy all of the SDL header files into it
  - Copy that dll file into your `release` folder
- Download the Windows port of dosfstools (mkdosfs) from [here](https://www1.mager.org/mkdosfs/) and put the exe in your `release` folder
- Download the sqlite precompiled binaries for windows from https://sqlite.org/download.html and put the dll file in your `release` folder
- Download the sqlite source code amalgamation and put the `sqlite3.h` header file in your `include` folder
- Build `sqlite3.lib` by following the instructions here https://gist.github.com/zeljic/d8b542788b225b1bcb5fce169ee28c55 and put it in your `lib` folder
- Download the standalone command line version of 7zip from https://www.7-zip.org/download.html
  - Copy `7za.exe`, `7za.dll`, and `7zxa.dll` into your `release` folder
- Create a `winfonts` folder in your checkout directory
- Download (or copy from a Linux install) the Noto Sans font family and the NotoMono font, and store the ttf font files in the `winfonts` folder
  - The font files should be: `NotoSans-Regular.ttf`, `NotoSans-Bold.ttf`, `NotoSans-Italic.ttf`, `NotoSans-BoldItalic.ttf`, and `NotoMono-Regular.ttf`
- Download the discord-rpc library from https://github.com/discord/discord-rpc/releases
  - Copy the lib into your `win64/lib` (or `win32/lib`) folder
  - Copy the dll into your `release` folder
- Open the `app.pro` file in Qt Creator
- Set the build directory to your checkout directory and set your build configuration to release (or debug)
- Build the project for the first time in Qt Creator
- Run `windeployqt release/parallel-launcher.exe`
- Copy the C++ runtimes DLLs from your system to the `release` directory (they will be located in a directory path something like `C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Redist\MSVC\14.28.29325\x64`)
- Copy the libcrypto and libssl dlls into your `release` directory from `C:\Qt\Tools\OpenSSL\Win_x64\bin` (64-bit) or `C:\Qt\Tools\OpenSSL\Win_x86\bin` (32-bit)
- Optionally, you can also build the style plugins to make more themes available (see below)
- Run `windows-deploy-translations.ps1`

#### Building
- If you made changes to the ui files or changed build configurations, run qmake again
- Build the project from Qt Creator.
- In the release directory, run `C:\Qt\5.15.2\msvc2019_64\bin\windeployqt.exe` (64-bit) or `C:\Qt\5.15.2\msvc2019\bin\windeployqt.exe` (32-bit)
- If the translations changed, run `windows-deploy-translations.ps1` in the root directory

#### Building the Styles/Themes (Optional)
- Clone the repository hosted at https://github.com/qt/qtstyleplugins into some temporary directory
- Open the project file in Qt Creator
- Go to the Projects tab and set the build configuration to Release
- Run qmake and build from Qt Creator
- Copy the built plugin DLL files to the `style` directory in your debug/release folder for parallel-launcher

#### Building the Installer
- Download and install Inno Setup (https://jrsoftware.org/isinfo.php)
- Open win64-installer.iss (or win32-installer.iss for a 32-bit build) in the source directory
- Click the green play button to build the installer
