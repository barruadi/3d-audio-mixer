# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

```bash
# Configure (from project root, first time only)
mkdir -p build && cd build && cmake .. -G Ninja

# Build / rebuild after changes
cd build && ninja

# Run the application (must be run from the project root —
# shaders are loaded via the relative path src/shaders/)
./build/bin/3D_Audio_Mixer
```

Build system uses CMake with the Ninja generator (`-G Ninja` is required; the default generator on macOS is Unix Makefiles). Output binary goes to `build/bin/`. Never run `cmake` in the project root — an in-source `CMakeCache.txt` breaks subsequent configures in `build/`.

```bash
# Build and run tests
cd build && ninja 3D_Audio_Mixer_Tests && ctest --output-on-failure

# Full GTest output
./build/bin/3D_Audio_Mixer_Tests --gtest_color=yes
```

GoogleTest is fetched automatically by CMake via FetchContent — no manual install. The test binary links against OpenGL and MiniAudio but runs headless; tests must not call any GL functions or initialize audio devices. Test files live in `src/test/` mirroring `src/3d_audio_mixer/` and use `.test.cc` extension.

## Architecture Overview

This is a 3D audio mixer application that allows positioning sound sources in 3D space with spatial audio.

### Technology Stack
- **C++17** with OpenGL rendering
- **MiniAudio** - single-header audio library for 3D spatial audio
- **ImGui** - immediate mode GUI
- **GLFW** - window management
- **GLM** - math library

### Core Architecture

```
Application (singleton)
└── Window (GLFW + OpenGL)
    ├── AudioContext (ma_engine) ─── shared across app
    ├── SceneView (3D viewport)
    │   ├── Camera (orbit/pan/zoom)
    │   ├── Grid (coordinate axes)
    │   └── SoundNode[] ─── each has AudioSystem + ma_sound
    ├── NodeInfo (selected node panel)
    └── MenuPanel (file browser)
```

### Key Namespaces
- `naudio` - Audio engine wrappers (`AudioContext`, `AudioSystem`)
- `nelement` - Scene elements (`SoundNode`, `Camera`, `Grid`, `Listener`)
- `nrender` - OpenGL rendering (`OpenGL_Context`, `UI_Context`)
- `nui` - ImGui panels (`SceneView`, `MenuPanel`, `NodeInfo`)
- `nwindow` - Window management
- `nshader` - Shader management

### Audio System Design
- `AudioContext` wraps `ma_engine` (one per app), manages the listener position
- `AudioSystem` manages multiple `ma_sound` instances with ID-based access (one per SoundNode)
- Sounds are loaded once and cached by file path
- 3D spatialization uses inverse distance attenuation model

### Conventions
- Member variables prefixed with `m` (e.g., `mEngine`, `mSoundId`)
- Header guards use `#pragma once`
- Smart pointers: `shared_ptr` for shared ownership, `unique_ptr` for exclusive
- Logging: `std::cout` for info, `std::cerr` for errors with `[INFO]`/`[ERROR]` prefixes

## Sub-Agents

Three Claude Code sub-agents are defined in `.claude/agents/`:

| File | Purpose |
|------|---------|
| `test-writer.md` | Write GTest unit tests in `src/test/`, build, iterate until all pass |
| `reviewer.md` | Review any code diff for correctness, style, resource safety |
| `commit.md` | Stage and commit following `COMMITS.md` format and atomicity rules |

Reference an agent in a prompt with `@.claude/agents/<name>.md`. Commit format: `[<type>] <description>` (see `COMMITS.md` for types and examples).
