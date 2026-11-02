# Building Azulindo

## Raspberry Pi OS (64-bit) and other Linux systems

Install the build tools and desktop OpenGL/X11 development libraries:

```sh
sudo apt update
sudo apt install build-essential cmake git pkg-config \
  libgl1-mesa-dev libx11-dev libxrandr-dev libxinerama-dev \
  libxcursor-dev libxi-dev
```

From the repository root, configure and build with the standard CMake/Make
workflow:

```sh
mkdir -p build
cd build
cmake ..
make -j2
```

CMake defaults to an optimized Release build. When configured directly on the
Raspberry Pi, `llama.cpp` detects the local processor and enables its native CPU
optimizations. The executable is written to `build/bin/azulindo_app`.

For a Raspberry Pi without a desktop environment, raylib can instead use its
DRM backend. Install the additional development packages and configure in a
separate build directory so the desktop build remains unchanged:

```sh
sudo apt install libdrm-dev libgbm-dev libegl1-mesa-dev libgles2-mesa-dev
cmake -S . -B build-drm -DPLATFORM=DRM -DCMAKE_BUILD_TYPE=Release
cmake --build build-drm -j2
```

The DRM build uses a 704x512 application canvas. Raylib selects a compatible
display mode from the modes exposed by the DRM connector.
