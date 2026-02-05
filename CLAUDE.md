# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a minimal OpenGL demo repository that renders an orange triangle. It includes both Python and C++ implementations of the same demo application.

## Build and Run Commands

### Python Version
```bash
# Install dependencies
pip install -r requirements.txt

# Run the demo
python3 opengl_demo.py
```

### C++ Version
```bash
# Build the C++ version (requires CMake)
mkdir -p build
cd build
cmake ..
make

# Run the compiled binary
./opengl_demo
```

**Note on macOS**: If CMake is installed as an application, the path is typically `/Applications/CMake.app/Contents/bin/cmake`.

**VS Code users**: If you encounter "CMake executable error", create `.vscode/settings.json`:
```json
{
    "cmake.cmakePath": "/Applications/CMake.app/Contents/bin/cmake"
}
```

## Architecture

### Project Structure

```
opengl_demo/
├── include/              # Header files
│   └── shader/
│       └── Shader.h      # CShader class header
├── src/                  # Source files
│   ├── main.cpp          # Main application entry
│   └── shader/
│       └── Shader.cpp    # CShader class implementation
├── resources/            # Resource files
│   ├── shaders/          # GLSL shader files
│   │   ├── *.vs         # Vertex shaders
│   │   ├── *.fs         # Fragment shaders
│   │   └── *.gs         # Geometry shaders (future)
│   ├── textures/         # Texture images (future)
│   └── models/           # 3D model files (future)
├── third_party/          # Third-party libraries
├── build/               # Build output directory
└── CMakeLists.txt       # Build configuration
```

### Dual Implementation Structure

The project maintains two parallel implementations:

- **Python version** (`opengl_demo.py`): Uses PyGame and PyOpenGL, targets OpenGL 1.2 (legacy shader version)
- **C++ version** (`src/main.cpp`): Uses GLFW and GLAD, targets OpenGL 3.3 Core Profile (modern)

Both implementations follow the same rendering pipeline:
1. Window creation and context initialization
2. Vertex buffer setup with triangle coordinates
3. Shader compilation (vertex and fragment shaders)
4. Render loop with event handling

### Key Differences Between Implementations

**Python**:
- Window management via PyGame
- Simplified shader compilation using `OpenGL.GL.shaders`
- GLSL version 120 (older syntax)
- No VAO usage (legacy approach)

**C++**:
- Window management via GLFW
- Manual shader compilation with error checking
- GLSL version 330 core (modern syntax)
- Uses VAO (Vertex Array Objects) for state management
- Platform-specific linking (macOS frameworks, Linux libraries, Windows static libs)

### Third-Party Dependencies

Located in `third_party/`:
- **GLAD** (OpenGL loader): Provides OpenGL function pointers for modern OpenGL
- **GLFW 3.3.8**: Cross-platform window and context creation library

Pre-built static libraries are in `third_party/lib/`:
- `libglad.a` - GLAD OpenGL loader
- `libglfw3.a` - GLFW library

Headers are in `third_party/include/`:
- `glad/glad.h` - GLAD header
- `GLFW/glfw3.h` - GLFW header
- `KHR/khrplatform.h` - Khronos platform types

### CMake Build Configuration

The CMakeLists.txt includes platform-specific linking:
- **macOS**: Links Cocoa, IOKit, and CoreVideo frameworks, plus glfw3 and glad static libraries
- **Linux**: Links glfw3, glad, dl, and pthread
- **Windows**: Links glfw3 and glad

All builds require OpenGL system libraries via `find_package(OpenGL REQUIRED)`.

**Important**: The build order in CMakeLists.txt is critical:
1. `include_directories()` and `link_directories()` must come before `add_executable()`
2. `add_executable()` must come before `target_include_directories()` and `target_link_libraries()`
3. Both `glfw3` and `glad` libraries must be explicitly linked on all platforms

### Shader Architecture

**C++ Version**:
- Uses modular `CShader` class for shader management
- Shaders loaded from external files in `resources/shaders/` directory
- Shader files use standard extensions: `.vs` (vertex), `.fs` (fragment), `.gs` (geometry)
- **CShader class** (`include/shader/Shader.h`):
  - Two constructors: one for inline shader strings, one for file paths
  - Methods: `use()`, `setBool()`, `setInt()`, `setFloat()` for shader usage
  - Automatic resource management via destructor
  - Comprehensive error handling for compilation and linking failures
- **Example usage**:
  ```cpp
  // Load from files
  CShader shader(std::string("resources/shaders/triangle.vs"), 
                std::string("resources/shaders/triangle.fs"));
  shader.use();
  ```

**Python Version**:
- Uses inline GLSL shaders
- Simplified shader compilation using `OpenGL.GL.shaders`
- GLSL version 120 (older syntax)

**Current shaders**:
- **Vertex shader** (`triangle.vs`): Passes through position attributes
- **Fragment shader** (`triangle.fs`): Outputs solid orange color (RGB: 1.0, 0.5, 0.2)

The CMakeLists.txt automatically copies the `resources/` directory to the build output during build, ensuring shader files are available at runtime.
