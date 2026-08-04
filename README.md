# 3d-audio-mixer

An open-source 3D audio mixer for creators, developers, and musicians to intuitively position and mix sounds in a virtual 3D space.

<br/>

## Build & Run

Prerequisites: CMake (>= 3.26) and Ninja (`brew install cmake ninja`). GLFW is bundled in `lib/`, everything else is vendored in `dependencies/`. Currently macOS only.

```bash
# Configure (first time only)
mkdir -p build && cd build && cmake .. -G Ninja

# Build / rebuild
cd build && ninja

# Run (from the project root)
./build/bin/3D_Audio_Mixer
```

Note: run the app from the project root — shaders are loaded via the relative path `src/shaders/`.

<br/>

## Tests

Tests use [Google Test](https://github.com/google/googletest), fetched automatically by CMake on first configure. No extra install required.

```bash
# Configure (first time only — downloads GoogleTest)
mkdir -p build && cd build && cmake .. -G Ninja

# Build tests
cd build && ninja 3D_Audio_Mixer_Tests

# Run all tests with pass/fail summary
cd build && ctest --output-on-failure

# Run test binary directly for full GTest output
./build/bin/3D_Audio_Mixer_Tests --gtest_color=yes
```

Test files live in `src/test/` mirroring the source structure and use `.test.cc` extension:

```
src/test/
├── elements/
│   ├── camera.test.cc
│   ├── listener.test.cc
│   └── sound_node.test.cc
└── utils/
    ├── file_io.test.cc
    └── scene_loader.test.cc
```

<br/>

## Claude Agents

Three sub-agents are defined in `.claude/agents/` for common workflows:

| Agent | Invocation | Purpose |
|-------|-----------|---------|
| `test-writer` | `@.claude/agents/test-writer.md` | Write and validate GTest unit tests |
| `reviewer` | `@.claude/agents/reviewer.md` | Review code changes for correctness and style |
| `commit` | `@.claude/agents/commit.md` | Commit following the COMMITS.md conventions |

<br/>

## Current Technology Stack

| Layer | Library/Tool |
|---|---|
| UI | ImGui / GLAD |
| 3D Viewport | OpenGL |
| Math | GLM |
| 3D Audio (real-time) | MiniAudio |
| Scene Graph | Custom + nlohmann/json |
| Window/Input | GLFW |
| Asset & Project Save | std::filesystem + JSON |
| Unit Testing | Google Test (GTest) |

<br/>

## Project Structure

```
3d-audio-mixer/
├── CMakeLists.txt
├── CLAUDE.md
├── COMMITS.md
├── src/
│   ├── main.cc
│   ├── glad.c
│   ├── shaders/
│   │   ├── node_frag.shader
│   │   └── node_vert.shader
│   ├── 3d_audio_mixer/
│   │   ├── app/          — Application singleton + PCH
│   │   ├── audio/        — AudioContext (ma_engine) + AudioSystem (ma_sound)
│   │   ├── elements/     — Camera, Grid, Listener, SoundNode
│   │   ├── render/       — OpenGL buffer + frame buffer managers
│   │   ├── shader/       — Shader load/compile/link
│   │   ├── ui/           — ImGui panels (SceneView, MenuPanel, NodeInfo, TimelinePanel)
│   │   ├── utils/        — FileIO, SceneLoader, Services
│   │   └── window/       — GLFW window wrapper
│   └── test/
│       ├── elements/     — camera, listener, sound_node tests
│       └── utils/        — file_io, scene_loader tests
├── dependencies/         — vendored: glad, GLFW, GLM, ImGui, MiniAudio, nlohmann
├── lib/                  — libglfw.3.4.dylib
└── build/                — cmake output (gitignored)
```
