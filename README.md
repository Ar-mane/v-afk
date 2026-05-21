# V-AFK

Minimal Win32 + DirectX 11 + Dear ImGui desktop app with a Valorant-inspired UI and a threaded anti-AFK bot.

## Stack

- C++17
- Win32
- DirectX 11
- Dear ImGui
- CMake
- Makefile helpers

## Project Structure

```text
src/
  main.cpp       UI, Win32 window, DX11, ImGui render loop
  bot.cpp        bot worker thread and input logic
  bot.h          bot interface
  app_icon.rc    Windows icon resource
  app_icon.h     icon resource id
assets/fonts/    custom UI fonts
app.png          app artwork
app.ico          Windows executable icon
CMakeLists.txt   build definition
Makefile         basic development commands
```

## Prerequisites

On Windows, make sure you have:

- Visual Studio Build Tools or Visual Studio with C++ support
- CMake
- `make` available in your shell

This project is intended to build on Windows.

## Setup

If you already have the repo and dependencies in place, you can go straight to build.

Basic flow:

```bash
make configure
make build
make run
```

## Development Commands

Use the Makefile for the basic workflow:

```bash
make help
```

Available commands:

- `make configure` : generate CMake project files into `build/`
- `make build` : build Release
- `make build-debug` : build Debug
- `make build-release` : build Release
- `make run` : build Release and launch the app
- `make clean` : remove the `build/` folder

## Direct CMake Commands

If you prefer not to use the Makefile:

```bash
cmake -S . -B build
cmake --build build --config Release
```

Release executable output:

```text
build/Release/V-AFK.exe
```

## Notes

- `main.cpp` is responsible for UI and rendering only.
- `bot.cpp` handles the threaded anti-AFK behavior.
- The EXE icon is provided through the Windows resource file.
- Custom fonts are loaded from `assets/fonts`.

## Return-To-Project Workflow

If you come back later and just want to continue working:

```bash
make build-debug
```

Or if you want to run the app quickly:

```bash
make run
```

## Contributing

Ideas, fixes, UI upgrades, and cleanup are always welcome.

Feel free to contribute and make it even cooler :)

happy coding 💻 ...

