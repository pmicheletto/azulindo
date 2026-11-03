# Azulindo

Azulindo is an offline AI companion application written in C++20, built with
raylib and llama.cpp.

## Setup and build

### Requirements

On Debian or Ubuntu, install a C++20 compiler, CMake 3.25 or newer, Git, and
Linux desktop development libraries:

```sh
sudo apt update
sudo apt install build-essential cmake git pkg-config \
  libgl1-mesa-dev libx11-dev libxrandr-dev libxinerama-dev \
  libxcursor-dev libxi-dev
```

Clone the repository and its dependencies:

```sh
git clone --recurse-submodules https://github.com/pmicheletto/azulindo.git
cd azulindo
```

If the submodules were not cloned, initialize them with:

```sh
git submodule update --init --recursive
```

Configure and build from the repository root:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel 2
```

The executable is created at `build/bin/azulindo_app`. See [BUILDING.md](BUILDING.md)
for Raspberry Pi and DRM-specific instructions.

## Run

From the repository root, run the app with an available GGUF model:

```sh
./build/bin/azulindo_app path/to/model.gguf
```

When no model path is provided, the application uses its configured default,
`Qwen3-1.7B-Q4_K_M.gguf`. If that file is not available, pass the path to a
compatible local model. Model weights are distributed under their own terms;
check those terms before redistributing them.

### Download the default model

The model file is about 1.3 GB and is intentionally not stored in this Git
repository. From the repository root, download the Q4_K_M quantization from the
`ggml-org` Hugging Face repository:

```sh
mkdir -p models
curl -L --fail --output models/Qwen3-1.7B-Q4_K_M.gguf \
  "https://huggingface.co/ggml-org/Qwen3-1.7B-GGUF/resolve/main/Qwen3-1.7B-Q4_K_M.gguf"
```

The model repository identifies the model as Apache-2.0 licensed. Review the
[model card and license](https://huggingface.co/ggml-org/Qwen3-1.7B-GGUF) before
redistributing the weights. Model downloads are not part of CI; CI only
configures and compiles the application.

## Authorship

Azulindo is an original project created and developed by
[Patrícia Micheletto](https://github.com/pmicheletto). Third-party dependencies,
models, and assets remain the work of their respective authors and are subject
to their own licenses.

## Licensing

The original Azulindo source code is licensed under the Mozilla Public License
2.0; see [LICENSE](LICENSE). The license permits commercial use and
distribution, while requiring source availability under MPL 2.0 for covered
files that are modified or redistributed. Third-party submodules, model
weights, and assets may have separate licenses and are not relicensed by this
project license.
