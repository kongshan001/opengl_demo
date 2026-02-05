# Resources Directory

This directory contains all resource files used by the OpenGL demo application.

## Structure

### shaders/
Contains GLSL shader source files:
- `*.vs` - Vertex shaders
- `*.fs` - Fragment shaders
- `*.gs` - Geometry shaders (future)

### textures/
Contains texture image files:
- `*.png` - PNG format images
- `*.jpg` - JPEG format images
- `*.tga` - TGA format images

### models/
Contains 3D model files (future):
- `*.obj` - OBJ model format
- `*.fbx` - FBX model format

## Usage

Shader files are loaded using the CShader class:

```cpp
// Load shaders from files
CShader shader("resources/shaders/triangle.vs", "resources/shaders/triangle.fs");
```

The CMakeLists.txt automatically copies the resources directory to the build output folder during build.
