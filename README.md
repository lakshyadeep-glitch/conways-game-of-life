# Conway's Game of Life

A dark-themed, interactive Conway's Game of Life simulator built with **C++17**, **SFML 3.x**, and **CMake**. Draw cells, run generations, explore classic patterns, and adjust the simulation speed in a native desktop window.

## Features

- Click cells to toggle them alive or dead.
- Implements Conway's four rules using a separate next-generation grid.
- Start, pause, step, clear, and randomize controls.
- Live generation and population counters.
- Adjustable cell size and simulation speed (1–30 generations per second).
- Pattern library: Glider, Pulsar, and Gosper Glider Gun.
- Pauses automatically while the pattern menu is open.
- Dark interface with green living cells.
- Window resizing adjusts the grid dimensions while preserving cells that still fit.
- CMake downloads SFML 3.0.2 automatically during configuration.

## Requirements

- Windows 10 or Windows 11
- Visual Studio Code
- CMake 3.24 or newer
- Git (needed by CMake FetchContent)
- A C++17 compiler:
  - MinGW-w64 GCC, or
  - MSVC from Visual Studio Build Tools

**Important:** use a consistent compiler toolchain. If you build with MinGW, use a MinGW-compatible CMake generator. If you build with MSVC, use the Visual Studio developer environment.

## Project structure

```text
GameOfLife/
├── CMakeLists.txt
├── README.md
├── .gitignore
├── include/
│   ├── Application.hpp
│   ├── GameOfLife.hpp
│   └── Renderer.hpp
├── src/
│   ├── main.cpp
│   ├── Application.cpp
│   ├── GameOfLife.cpp
│   └── Renderer.cpp
└── assets/
```

## Build on Windows

### 1. Install the tools

Install:
- VS Code: https://code.visualstudio.com/
- CMake: https://cmake.org/download/
- Git: https://git-scm.com/download/win/
- A C++ compiler:
  - MinGW-w64 via MSYS2: https://www.msys2.org/
  - Or Visual Studio Build Tools: https://visualstudio.microsoft.com/visual-cpp-build-tools/

For MSYS2 MinGW 64-bit, open the **UCRT64** terminal and install the compiler and tools:

```bash
pacman -S --needed mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-ninja git
```

Add `C:\msys64\ucrt64\bin` to your Windows `PATH`, then restart VS Code. Check:

```powershell
g++ --version
cmake --version
git --version
```

### 2. Open the project

Extract the project ZIP, then open the `GameOfLife` folder in VS Code. Open **Terminal → New Terminal**.

### 3. Configure and build (MinGW Makefiles)

If your CMake and compiler are both from a compatible MinGW installation:

```powershell
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER=g++
cmake --build build --config Release
```

If you installed Ninja with MSYS2, use this alternative instead:

```powershell
cmake -S . -B build -G Ninja -DCMAKE_CXX_COMPILER=g++
cmake --build build --config Release
```

The first configure step downloads SFML 3.0.2, so it needs an internet connection. It may take a few minutes.

### 4. Run

```powershell
.\build\GameOfLife.exe
```

For a multi-configuration Visual Studio generator, the executable may instead be located at:

```powershell
.\build\Release\GameOfLife.exe
```

## Controls

| Action | How |
|---|---|
| Toggle a cell | Left-click a cell |
| Start / pause | Start button or `Space` |
| Advance one generation | Step button or `N` |
| Randomize | Randomize button or `R` |
| Clear the board | Clear button or `C` |
| Open pattern library | Choose a pattern |
| Close pattern menu | Close menu or `Esc` |
| Adjust speed | Drag the speed slider |
| Change cell size | Smaller / Larger buttons |
| Pause | `Esc` |

## Classic patterns

- **Glider:** a small pattern that moves diagonally over time.
- **Pulsar:** a periodic oscillator.
- **Gosper Glider Gun:** a pattern that repeatedly emits gliders.

Choosing a pattern clears the current board and places the selected seed near the center.

## Screenshots

Add screenshots here after running the application. For example:

```markdown
![Main simulation window](assets/screenshot.png)
```

## Troubleshooting

### `cmake` is not recognized

Re-run the CMake installer and enable adding CMake to `PATH`, or add its `bin` directory manually. Restart VS Code after changing `PATH`.

### `g++` is not recognized

Add your compiler's `bin` directory to `PATH`. For the MSYS2 UCRT64 toolchain, this is commonly `C:\msys64\ucrt64\bin`.

### CMake cannot find Git or cannot clone SFML

Install Git, restart VS Code, check `git --version`, and retry the configure command with an internet connection. A proxy or firewall may block GitHub access.

### Compiler or generator mismatch

CMake caches the selected compiler in the build directory. If you switch between MinGW and MSVC, delete the `build` folder and configure again with the appropriate generator.

### SFML API errors

This project targets SFML 3.0.2. Make sure CMake has fetched that version and avoid mixing in SFML 2.x headers or examples.

### Font or text is missing

The renderer tries standard Windows fonts at `C:\Windows\Fonts\segoeui.ttf` and `arial.ttf`. If neither is available, the simulation still opens but text labels will not render.

## How the simulation works

For each cell, the engine counts the eight surrounding positions. A live cell survives with two or three live neighbors; otherwise it dies. A dead cell becomes alive only when it has exactly three live neighbors. The engine writes all outcomes into a second grid and swaps the grids after the generation is complete, avoiding premature updates.

## License

This project is provided for learning and personal portfolio use. You may add a license such as MIT before publishing it publicly.
