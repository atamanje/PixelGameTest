# PixelGameTest

A modular, lightweight 2D game engine and architectural testbed written in C++20 using OpenGL, GLFW, Glad, and Dear ImGui.

`PixelGameTest` is designed as a hands-on exploration of fundamental 2D game mechanics, game loop architecture, player controllers, tilemaps, sprite animation, and interactive branching dialogue systems.

---

## 🎮 Vision & Scope

The goal of `PixelGameTest` is to provide a clean, highly extensible 2D RPG/adventure game sandbox. It emphasizes clear object-oriented design and gradual feature expansion:

- **2D World & Tilemap**: Grid-based environment rendering with tile collisions (grass, water, paths, solid walls).
- **Player & NPC Controllers**: 4-directional movement, facing vectors, bounding box collision (AABB), and wall-sliding physics.
- **Dynamic 2D Camera**: World-to-screen coordinate transformations, zoom controls, and smooth target tracking.
- **Branching Dialogue System**: Node-based conversation engine supporting speaker tags, choice selections, and event callbacks.
- **Sprite & Texture Pipeline**: Built-in 2D texture loading and frame-based sprite animation system.
- **Developer Workstation**: Live ImGui docking UI featuring a viewport canvas, real-time entity inspector, camera controls, and debug overlays.

---

## 🛠️ Architecture Overview

The codebase follows a clean object-oriented architecture:

```
                          ┌─────────────────────────┐
                          │       GameManager       │
                          └────────────┬────────────┘
                                       │
            ┌──────────────────────────┼──────────────────────────┐
            │                          │                          │
   ┌────────▼────────┐        ┌────────▼────────┐        ┌────────▼────────┐
   │     World       │        │    Camera2D     │        │ DialogueSystem  │
   │ (Grid & Tiles)  │        │ (World↔Screen)  │        │ (Tree & Choice) │
   └────────┬────────┘        └─────────────────┘        └─────────────────┘
            │
    ┌───────┴───────────────┐
    │ Entity (Abstract Base)│
    └───────┬───────────────┘
            │
    ┌───────┴───────────────┐
    │       Character       │
    └───────┬───────────────┬
            │               │
    ┌───────▼───────┐ ┌─────▼─────────┐
    │    Player     │ │      NPC      │
    │ (WASD Input)  │ │(Elder/Merchant)│
    └───────────────┘ └───────────────┘
```

- **`Vec2`**: 2D vector math helper struct (distance, normalization, length).
- **`Camera2D`**: Manages world camera position, viewport conversions, and smooth target tracking.
- **`Entity`**: Abstract base class for all in-world objects with position, bounding box collision, and render interfaces.
- **`Character`**: Derived from `Entity`; adds movement speed, 4-directional facing, and collision-sliding physics.
- **`Player`**: Player-controlled entity with WASD input reading and NPC proximity interaction triggers.
- **`NPC`**: Interactive non-player characters with name tags, custom tint colors, and dialogue triggers.
- **`Texture2D` & `AnimatedSprite`**: Handles 2D image loading via `stb_image` and sprite animation frame management.
- **`World`**: Grid tilemap renderer, tile collision checks, and entity spatial queries.
- **`DialogueSystem`**: Manages node-based dialogue trees, text rendering, and choice branching.
- **`GameManager`**: Central state coordinator handling updates, camera tracking, viewport clipping, and UI overlays.

---

## 📦 Dependencies

All third-party libraries are bundled in the `vendor/` directory for zero-setup compilation:

| Dependency | Purpose | Source |
| :--- | :--- | :--- |
| **C++20** | Programming language standard | MSVC / Compiler |
| **GLFW 3.3** | Window creation, OpenGL context & raw input | `vendor/glfw/` |
| **Glad** | OpenGL 3.3 Core profile function loader | `vendor/glad/` |
| **Dear ImGui** | Developer UI, docking workspace & viewport overlay | `vendor/imgui/` |
| **stb_image** | Image loading (`.png`, `.jpg`, `.bmp`) | `vendor/stb_image/` |
| **GoogleTest** | C++ unit testing framework | `vendor/googletest/` |
| **Premake5** | Cross-platform build configuration generator | `vendor/premake/` |

---

## 🚀 Getting Started

### Prerequisites
- **OS**: Windows (x64)
- **Compiler**: Visual Studio 2022 with Desktop development with C++ workload (MSVC v143)

### Building the Project

1. **Generate Visual Studio Solution**:
   Run the project generator script from the repository root:
   ```cmd
   vendor\premake\GenWinProject.bat
   ```
   *This creates `PixelGameTest.sln`.*

2. **Compile via Batch Scripts**:
   - **Debug Build**:
     ```cmd
     build_debug.bat
     ```
   - **Release Build**:
     ```cmd
     build_release.bat
     ```
   - **Rebuild Commands**:
     ```cmd
     rebuild_debug.bat
     rebuild_release.bat
     ```

3. **Run Unit Tests**:
   After compiling, run the GoogleTest binary:
   ```cmd
   bin\Debug-windows-x86_64\Tests\Tests.exe
   ```

---

## 🕹️ Controls & Navigation

- **`W`, `A`, `S`, `D`** / **Arrow Keys**: Move the Hero character.
- **`Space`** / **`Enter`**: Interact with nearby NPCs or advance dialogue.
- **Mouse Drag / Sliders**: Adjust camera zoom, move speed, and inspect entity positions in the **Controls** panel.

---

## 📁 Repository Structure

```
PixelGameTest/
├── build_debug.bat         # Fast Debug build script
├── build_release.bat       # Release build script
├── premake5.lua            # Premake build configuration file
├── README.md               # Project documentation
├── resources/              # Application icon and static assets
├── src/                    # Core C++ engine source code
│   ├── AnimatedSprite.h/cpp
│   ├── Application.h/cpp
│   ├── Camera2D.h/cpp
│   ├── Character.h/cpp
│   ├── DialogueSystem.h/cpp
│   ├── Entity.h/cpp
│   ├── GameManager.h/cpp
│   ├── main.cpp
│   ├── NPC.h/cpp
│   ├── pch.h/cpp
│   ├── Player.h/cpp
│   ├── Texture2D.h/cpp
│   ├── Vec2.h
│   └── World.h/cpp
├── tests/                  # GoogleTest unit test files
│   └── MainTests.cpp
└── vendor/                 # Third-party dependencies (GLFW, Glad, ImGui, etc.)
```

---

## 👤 Author

- **Jessie Atamanchuk**
- **License**: MIT / Open Experimentation
