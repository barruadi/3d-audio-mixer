# 3d-audio-mixer
will be an open-source 3D audio mixer designed for creators, developers, and musicians to intuitively position and mix sounds in a virtual 3D space. 

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

## Future Technology Usage

| Layer                | Library/Tool           |
| -------------------- | ---------------------- |
| UI                   | ImGui / GLAD           |
| 3D Viewport          | OpenGL                 |
| Math                 | glm                    |
| 3D Audio             | FMOD                   |
| Audio File Loading   | libsndfile / dr\_wav   |
| Scene Graph          | Custom + nlohmann      |
| Window/Input         | GLFW / SDL2            |
| Asset & Project Save | std::filesystem + JSON |
| Mixer / Timeline     | ImGui tables / JUCE    |

__\* Still Approximations \*__

<br/>

## Overall Project Structure

```sh
*
├── CMakeLists.txt
├── main.cc
│
├── assets/
│   └── sounds/
│
├── build/
│
├── src/
│   └──3d_audio_mixer
│       ├── app/
│       │   ├── application.hhh/.cc
│       │   └── main.cc
│       │
│       ├── ui/
│       │   ├── ui_layer.cc
│       │   ├── docking_layout.cc
│       │   ├── properties_panel.cc
│       │   └── timeline_panel.cc
│       │
│       ├── render/
│       │   ├── renderer.hh/.cc
│       │   ├── camera.hh/.cc
│       │   ├── viewport.cc
│       │   └── gizmo.hh/.cc
│       │
│       ├── elements/
│       │   ├── listener.hh/.cc
│       │   ├── audio_node.hh/.cc
│       │   └── <future>
│       │
│       ├── audio/
│       │   ├── audio_engine.hh/.cc
│       │   ├── audio_source.hh/.cc
│       │   ├── listener.hh/.cc
│       │   ├── mixer.hh/.cc
│       │   └── audio_file_loader.hh/.cc
│       │
│       ├── window/
│       │   ├── window.hh/.cc
│       │   └── iwindow.hh
│       │
│       ├── core/
│       │   ├── scene.hh/.cc
│       │   ├── entity.hh/.cc
│       │   ├── transform.hh/.cc
│       │   └── component.hh/.cc
│       │
│       ├── utils/
│       │   ├── file_io.hh/.cc
│       │   ├── json_serializer.hh/.cc
│       │   ├── logger.hh/.cc
│       │   └── math_utils.hh/.cc
│       │
│       └── platform/
│           ├── window.hh/.cc
│           └── input.hh/.cc
│
├── include/
│   └── <future>
│
├── external/
│   ├── glfw/
│   ├── glad/
│   ├── imgui/
│   ├── glm/
│   ├── openal-soft/
│   └── nlohmann_json/
│
└── shaders/
    └── basic_shader.glsl
```